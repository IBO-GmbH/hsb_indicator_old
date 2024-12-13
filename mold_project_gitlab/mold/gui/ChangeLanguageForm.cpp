#include "ChangeLanguageForm.hpp"
#include <boost/format.hpp>
#include <boost/stacktrace.hpp>
#include "mold/version.hpp"
#include "ui_ChangeLanguageForm.h"

using namespace mold;
using namespace logging;

ChangeLanguageForm::ChangeLanguageForm(const WolfServices& services,
                                       LanguageHandler* language,
                                       QWidget* parent)
    : QWidget(parent),
      ui(new Ui::ChangeLanguageForm),
      m_services(services),
      m_logger("ChangeLanguageForm"),
      m_secretScreenCounter{0} {
  ui->setupUi(this);

  m_on_completion = [this] {
    setDisabled(false);
    emit back();
  };

  std::stringstream id;
  QString id_text(tr("Device ID: "));
  id << id_text.toStdString();
  constexpr int bits_to_output = 8 * 6;
  for (int base = 0; base < bits_to_output; base += 8) {
    const unsigned int to_print =
        (m_services.mac_address >> (bits_to_output - (base + 8))) & 0xff;
    id << boost::format("%02x") % to_print;
    if (base < bits_to_output - 8) {
      id << "-";
    }
  }
  std::stringstream version_;
  version_ << " v" << version::get_version_and_hash();
  ui->gatewayId->setText(QString::fromStdString(id.str()));
  ui->version->setText(QString::fromStdString(version_.str()));
  connect(ui->back, &QPushButton::clicked, this, &ChangeLanguageForm::back);
  if (language->getLanguage() == wolf::languages::english)
    ui->english->setDisabled(true);
  else {
    connect(ui->english, &QPushButton::clicked, this, [this, language] {
      language->setEnglish();
      changeRoomNames(wolf::languages::english);
    });
  }
  if (language->getLanguage() == wolf::languages::german)
    ui->german->setDisabled(true);
  else {
    connect(ui->german, &QPushButton::clicked, this, [this, language] {
      language->setGerman();
      changeRoomNames(wolf::languages::german);
    });
  }
  connect(ui->gatewayId, &QPushButton::clicked, this,
          &ChangeLanguageForm::pressedAddress);
}

ChangeLanguageForm::~ChangeLanguageForm() = default;

void ChangeLanguageForm::pressedAddress() {
  if (m_secretScreenCounter < 2) {
    ++m_secretScreenCounter;
    return;
  }
  LOGGING_LOG(m_logger, severity::normal) << "enter debug screen";
  emit debugStatusScreen();
}

void ChangeLanguageForm::changeRoomNames(const wolf::languages& language) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "changeRoomNames() stacktrace:" << boost::stacktrace::stacktrace();
  setDisabled(true);
  m_services.room_name_language_changer->async_set(language, m_on_completion);
}
