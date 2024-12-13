#include "StatusBarForm.hpp"
#include "mold/gui/GsmInformationReader.hpp"
#include "ui_StatusBarForm.h"
#include "wolf/esp3_parser.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/websocket_sync.hpp"

#include <QDateTime>
#include <QTimer>
#include <boost/optional/optional_io.hpp>

using namespace logging;
using namespace mold;

StatusBarForm::StatusBarForm(QWidget *parent)
    : QWidget(parent), m_logger{"StatusBarForm"}, ui(new Ui::StatusBarForm) {
  ui->setupUi(this);

  connect(ui->settings, &QToolButton::clicked, this,
          &StatusBarForm::openSettings);
  ui->sensor->hide();

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
  // TODO dirty - timer should get triggerd every "absolute" minute!
  timer->start(10000);  // time specified in ms
  updateTime();
}

StatusBarForm::~StatusBarForm() = default;

void StatusBarForm::initialise(const WolfServices &services,
                               const GsmInformationReader &informationReader) {
  wolf::thread_checker::throw_if_not_ui("StatusBarForm::initialise");

  disconnected();
  if (!services.websocket) {
    return;
  }
  if (services.websocket->get_state() ==
      wolf::websocket_sync::state::connected) {
    connected();
  }
  m_connection_connected =
      services.websocket->signal_connected.connect([this]() {
        QMetaObject::invokeMethod(this, "connected", Qt::QueuedConnection);
      });
  m_connection_disconnected =
      services.websocket->signal_closed.connect([this]() {
        QMetaObject::invokeMethod(this, "disconnected", Qt::QueuedConnection);
      });
  if (services.esp3_parser != nullptr) {
    m_blink = services.esp3_parser->signal_data_4bs.connect(
        [this](const wolf::types::id_esp3 &, const wolf::types::data &,
               const int) {
          QMetaObject::invokeMethod(this, "blinkForData", Qt::QueuedConnection);
        });
  }

  // rssi
  setRssi(informationReader.getLastRssi());
  updateGsmProvider(informationReader.getLastProvider());
  connect(&informationReader, &GsmInformationReader::signalRssi, this,
          &StatusBarForm::setRssi);
  connect(&informationReader, &GsmInformationReader::signalProvider, this,
          &StatusBarForm::updateGsmProvider);
}

QToolButton *StatusBarForm::getSettingsButton() const {
  if (ui->settings == nullptr)
    throw std::runtime_error(
        "StatusBarForm::getSettingsButton: settings not found");
  return ui->settings;
}

void StatusBarForm::connected() {
  QIcon icon;
  icon.addFile(QStringLiteral(":/settings/icons/connected.svg"), QSize(),
               QIcon::Normal, QIcon::Off);
  ui->connection->setIcon(icon);
}

void StatusBarForm::disconnected() {
  QIcon icon;
  icon.addFile(QStringLiteral(":/settings/icons/disconnected.svg"), QSize(),
               QIcon::Normal, QIcon::Off);
  ui->connection->setIcon(icon);
}

void StatusBarForm::blinkForData() {
  ui->sensor->show();
  QTimer::singleShot(250, ui->sensor, SLOT(hide()));
}

void StatusBarForm::updateTime() {
  QDateTime local(QDateTime::currentDateTime());
  QString result = local.toString(Qt::DefaultLocaleShortDate);
  ui->time->setText(result);
}

void StatusBarForm::setRssi(const std::optional<std::int32_t> rssi) {
  if (!rssi.has_value()) {
    LOGGING_LOG(m_logger, severity::verbose) << "read no rssi";
    ui->signal->hide();
    ui->gsm_provider->hide();
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose) << "setRssi, rssi:" << rssi.value();
  ui->signal->show();

  int index{5};
  if (rssi.value() < 30) {
    index = 4;
  }
  if (rssi.value() < 20) {
    index = 3;
  }
  if (rssi.value() < 10) {
    index = 2;
  }
  if (rssi.value() < 5) {
    index = 1;
  }
  if (rssi.value() < 2 || rssi.value() == 99) {
    index = 0;
  }
  const QString resource =
      ":/settings/icons/signal" + QString::number(index) + ".svg";
  QIcon icon;
  icon.addFile(resource, QSize(), QIcon::Normal, QIcon::Off);
  ui->signal->setIcon(icon);
}

void StatusBarForm::updateGsmProvider(const std::optional<QString> provider) {
  if (!provider) {
    LOGGING_LOG(m_logger, severity::verbose) << "read no provider";
    ui->gsm_provider->hide();
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << "read the following provider:\"" << provider.value().toStdString()
      << "\"";
  ui->gsm_provider->setText(provider.value());
  ui->gsm_provider->show();
}
