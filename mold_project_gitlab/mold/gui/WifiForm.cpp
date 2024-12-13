#include "WifiForm.hpp"
#include "ui_WifiForm.h"

#include "wifi/access_point_getter.hpp"

#include <QScrollBar>
#include <sstream>

using namespace logging;

mold::WifiForm::WifiForm(const mold::WolfServices &services,
                         mold::DataExchange *const dataExchange,
                         const access_point &to_connect,
                         const std::string &password, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WifiForm),
      m_logger("WifiForm"),
      m_services(services),
      m_dataExchange(dataExchange),
      m_previous_state(wifi::device_state::unknown),
      m_to_connect(to_connect),
      m_password(password),
      m_connected(optional_access_point()) {
  ui->setupUi(this);

  ui->popUp->setVisible(false);

  connect(ui->back, &QPushButton::clicked, this, &WifiForm::back);
  connect(ui->confirm, &QPushButton::clicked, this,
          &WifiForm::handleErrorConfirmed);

  const auto &access_points_ = dataExchange->get_access_points();
  m_access_points.reserve(access_points_.size());
  for (const auto &access_point : access_points_)
    m_access_points.push_back(access_point);
  makeList();

  connect(m_dataExchange, &DataExchange::access_point_added, this,
          &WifiForm::accessPointAdded);
  connect(m_dataExchange, &DataExchange::access_point_removed, this,
          &WifiForm::accessPointRemoved);
  connect(m_dataExchange, &DataExchange::device_state_changed, this,
          &WifiForm::deviceStateChanged);
  connect(ui->list, &QListWidget::itemClicked, this, &WifiForm::didChoose);

  if (m_to_connect.ssid.empty()) m_dataExchange->scan_wifi_networks();

  checkConnected();
  sortAccessPoints();
  if (!m_to_connect.ssid.empty()) handleConnect();
}

template <typename F>
static void postToThread(F &&fun, QObject *obj = qApp) {
  struct Event : public QEvent {
    F m_fun;
    Event(F &&fun_) : QEvent(QEvent::None), m_fun(fun_) {}
    ~Event() { m_fun(); }
  };

  QCoreApplication::postEvent(obj, new Event(std::move(fun)));
}

void mold::WifiForm::accessPointAdded(mold::WifiForm::access_point added) {
  postToThread([this, added] {
    LOGGING_LOG(m_logger, severity::verbose) << "access point added: " << added;
    auto item =
        new QListWidgetItem(QString::fromStdString(added.ssid), ui->list);
    setLockIcon(added, item);
    m_access_points.push_back(added);
  });
}

void mold::WifiForm::accessPointRemoved(std::string removed) {
  postToThread([this, removed] {
    LOGGING_LOG(m_logger, severity::verbose)
        << "removed access point with path: " << removed;
    auto found = findAccessPoint(removed);
    if (!checkFound(found)) return;
    m_access_points.erase(found);
    ui->list->clear();
    makeList();
    checkConnected();
    sortAccessPoints();
  });
}

void mold::WifiForm::deviceStateChanged(const wifi::device_state state) {
  postToThread([this, state] {
    LOGGING_LOG(m_logger, severity::verbose)
        << "device state changed: " << static_cast<int>(state);
    switch (state) {
      case wifi::device_state::unmanaged:
        handleUnmanaged();
        break;
      case wifi::device_state::unavailable:
        handleUnavailable();
        break;
      case wifi::device_state::disconnected:
        handleDisconnected();
        break;
      case wifi::device_state::prepare:
        handleConnecting(state);
        break;
      case wifi::device_state::config:
        handleConnecting(state);
        break;
      case wifi::device_state::need_auth:
        handleNeedAuth();
        break;
      case wifi::device_state::ip_config:
        handleConnecting(state);
        break;
      case wifi::device_state::ip_check:
        handleConnecting(state);
        break;
      case wifi::device_state::secondaries:
        handleConnecting(state);
        break;
      case wifi::device_state::activated:
        handleConnected();
        break;
      case wifi::device_state::deactivating:
        handleDeactivating();
        break;
      case wifi::device_state::failed:
        handleFailed();
        break;
      case wifi::device_state::unknown:
        handleUnknown();
        break;
    }
  });
}

void mold::WifiForm::didChoose() {
  std::size_t row = static_cast<std::size_t>(ui->list->currentRow());
  if (row >= m_access_points.size())
    throw std::runtime_error("WifiForm::didChoose: last row reached");
  auto chosen = m_access_points[row];
  emit accessPointChosen(chosen);
}

void mold::WifiForm::makeList() {
  for (const auto &access_point : m_access_points) {
    auto list_item = new QListWidgetItem(
        QString::fromStdString(access_point.ssid), ui->list);
    setLockIcon(access_point, list_item);
  }
}

void mold::WifiForm::handleUnmanaged() {
  LOGGING_LOG(m_logger, severity::warning) << "wifi device not connected!";
  m_previous_state = wifi::device_state::unmanaged;
}

void mold::WifiForm::handleUnavailable() {
  LOGGING_LOG(m_logger, severity::normal)
      << "could not connect to access point!";
  m_previous_state = wifi::device_state::unavailable;
}

void mold::WifiForm::handleDisconnected() {
  std::stringstream string;
  string << "disconnected, because ";
  if (m_previous_state == wifi::device_state::deactivating)
    string << "wifi device closed connection";
  if (m_previous_state == wifi::device_state::failed)
    string << "access point not reachable";
  LOGGING_LOG(m_logger, severity::normal) << string.str();
  m_previous_state = wifi::device_state::disconnected;
  ui->list->clear();
  makeList();
}

void mold::WifiForm::handleConnecting(const wifi::device_state state) {
  LOGGING_LOG(m_logger, severity::verbose) << "connecting, state: " << state;
  m_previous_state = state;
}

void mold::WifiForm::handleNeedAuth() {
  LOGGING_LOG(m_logger, severity::verbose) << "wrong password!";
  m_error = WifiErrorType::wrong_password;
  m_previous_state = wifi::device_state::need_auth;
}

void mold::WifiForm::handleConnected() {
  LOGGING_LOG(m_logger, severity::normal) << "successfully connected!";
  m_previous_state = wifi::device_state::activated;
  checkConnected();
  sortAccessPoints();
  m_services.service->post([this] { m_services.reconnect_handler->connect(); });
}

void mold::WifiForm::handleDeactivating() {
  LOGGING_LOG(m_logger, severity::verbose) << "wifi device closing connection";
  m_error = WifiErrorType::device_diconnect;
  m_previous_state = wifi::device_state::deactivating;
  handleIconChangeDisconnect();
}

void mold::WifiForm::handleFailed() {
  std::stringstream string;
  string << "failed, because ";
  if (m_previous_state == wifi::device_state::need_auth)
    string << "password is wrong";
  if (m_previous_state == wifi::device_state::config ||
      m_previous_state == wifi::device_state::ip_config) {
    string << "access point not reachable";
    m_error = WifiErrorType::access_point_not_reachable;
  }
  if (m_previous_state == wifi::device_state::activated) {
    string << "access point closed connection";
    m_error = WifiErrorType::access_point_disconnect;
  }
  LOGGING_LOG(m_logger, severity::verbose) << string.str();
  m_previous_state = wifi::device_state::failed;
}

void mold::WifiForm::handleUnknown() {
  LOGGING_LOG(m_logger, severity::warning) << "unknown wifi device state!";
  m_previous_state = wifi::device_state::unknown;
}

void mold::WifiForm::handleSuccessfulConnection(const access_point &connected) {
  setCheckIcon(QString::fromStdString(m_to_connect.ssid));
  m_connected = connected;
  setDisabled(false);
}

void mold::WifiForm::handleFailedConnection() {
  handleErrorMessage();
  ui->popUp->setVisible(true);
  ui->list->setEnabled(false);
  setDisabled(false);
}

void mold::WifiForm::handleErrorMessage() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handleErrorMessage error: " << static_cast<int>(m_error);

  switch (m_error) {
    case WifiErrorType::access_point_disconnect:
      changePopUp(tr("Access Point closed connection"));
      break;
    case WifiErrorType::access_point_not_reachable:
      changePopUp(tr("Access Point not reachable"));
      break;
    case WifiErrorType::device_diconnect:
      changePopUp(tr("WiFi device closed connection"));
      break;
    case WifiErrorType::wrong_password:
      changePopUp(tr("entered password is wrong"));
      break;
  }
  if (m_to_connect.needs_password && m_password.empty())
    changePopUp(tr("no password entered, \nalthough necessery"));
}

void mold::WifiForm::handleErrorConfirmed() {
  ui->popUp->setVisible(false);
  ui->list->setEnabled(true);
}

void mold::WifiForm::handleConnect() {
  auto found = findAccessPoint(m_to_connect.path);
  if (!checkFound(found)) return;

  access_point chosen = *found;
  auto success = [this, chosen]() {
    postToThread([this, chosen]() { handleSuccessfulConnection(chosen); });
  };
  auto fail = [this]() { postToThread([this] { handleFailedConnection(); }); };
  setDisabled(true);
  m_connector.reset(
      new wifi::connector(chosen.ssid, m_password, success, fail));
}

mold::WifiForm::access_points::iterator mold::WifiForm::findAccessPoint(
    const std::string &path_to_find) {
  return std::find_if(m_access_points.begin(), m_access_points.end(),
                      [&path_to_find](const access_point &to_check) {
                        return to_check.path == path_to_find;
                      });
}

void mold::WifiForm::setCheckIcon(const QString &ssid) {
  QIcon icon(":/settings/icons/checkmark");
  auto items = findItemInList(ssid);
  if (!items) return;
  items.get().front()->setIcon(icon);
}

void mold::WifiForm::checkConnected() {
  boost::optional<std::string> active_access_point =
      m_dataExchange->get_active_access_point();
  if (!active_access_point) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "not connected to an access point";
    m_connected = optional_access_point();
    return;
  }
  wifi::access_point_getter access_point_getter(active_access_point.get());
  const access_point access_point = access_point_getter.get();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "connected access_point: " << access_point;
  setCheckIcon(QString::fromStdString(access_point.ssid));
  m_connected = access_point;
}

boost::optional<QList<QListWidgetItem *>> mold::WifiForm::findItemInList(
    const QString &ssid) {
  auto items = ui->list->findItems(ssid, Qt::MatchCaseSensitive);
  if (items.empty()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "connected access point not in list";
    return boost::none;
  }
  return items;
}

void mold::WifiForm::setLockIcon(const mold::WifiForm::access_point &to_check,
                                 QListWidgetItem *to_set) {
  if (to_check.needs_password) {
    QIcon icon(":/settings/icons/lock_icon");
    to_set->setIcon(icon);
  }
}

void mold::WifiForm::handleIconChangeDisconnect() {
  if (!m_connected) return;
  auto items = findItemInList(QString::fromStdString(m_connected.get().ssid));
  if (!items) return;
  items.get().front()->setIcon(QIcon());
  setLockIcon(m_connected.get(), items.get().front());
}

void mold::WifiForm::sortAccessPoints() {
  if (!m_connected) return;
  auto items = findItemInList(QString::fromStdString(m_connected.get().ssid));
  if (!items) return;
  auto item = ui->list->takeItem(ui->list->row(items.get().front()));
  ui->list->insertItem(0, item);
  auto found = findAccessPoint(m_connected.get().path);
  if (!checkFound(found)) return;
  m_access_points.erase(found);
  m_access_points.insert(m_access_points.begin(), m_connected.get());
}

bool mold::WifiForm::checkFound(const access_points::iterator &found) {
  if (found == m_access_points.cend()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "checkFound: access point no longer in list!";
    return false;
  }
  return true;
}

void mold::WifiForm::changePopUp(const QString &error_text) {
  ui->errorText->setText(error_text);
  ui->errorText->adjustSize();
  const auto position_y_button =
      ui->errorText->y() + ui->errorText->height() + 20;
  ui->confirm->move(ui->confirm->x(), position_y_button);
  const auto new_height = ui->confirm->y() + ui->confirm->height() + 20;
  const auto position_x = (width() - ui->popUp->width()) / 2;
  const auto position_y =
      (height() + ui->settingLabel->height() - new_height) / 2;
  ui->popUp->setGeometry(
      {position_x, position_y, ui->popUp->width(), new_height});
}

mold::WifiForm::~WifiForm() = default;
