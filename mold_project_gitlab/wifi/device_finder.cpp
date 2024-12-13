#include "device_finder.hpp"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include "dbus_error.hpp"
#include "dbus_helper.hpp"
#include "org_desktop_networkmanager.h"
#include "org_desktop_networkmanager_device_wireless.h"

using namespace wifi;

static std::unique_ptr<OrgFreedesktopNetworkManagerInterface>
create_network_manager() {
  std::unique_ptr<OrgFreedesktopNetworkManagerInterface> result(
      new OrgFreedesktopNetworkManagerInterface(
          dbus_helper::network_service(), "/org/freedesktop/NetworkManager",
          QDBusConnection::systemBus()));
  return result;
}

device_finder::device_finder()
    : m_logger{"device_finder"}, m_networkmanager(create_network_manager()) {}

device_finder::~device_finder() = default;

void device_finder::ensure_connected() const {
  if (!QDBusConnection::systemBus().isConnected())
    throw std::runtime_error("!QDBusConnection::systemBus().isConnected()");
}

static void checkForError(const QDBusMessage& message) {
  // TODO
}

boost::optional<std::string> device_finder::get_first_wifi_device() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "get_first_wifi_device";
  QList<QDBusObjectPath> devices =
      dbus_helper::get_value_of_pending_reply(m_networkmanager->GetDevices());
  for (const auto& path : devices) {
    if (!is_device_wifi(path)) continue;
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "found a wifi device:" << path.path().toStdString();
    return path.path().toStdString();
  }
  return {};
}

bool device_finder::is_device_wifi(const QDBusObjectPath& path) {
  QString pathCasted = path.path();
  // LOGGING_LOG(m_logger, logging::severity::verbose) << "isDeviceWifi:" <<
  // pathCasted.toStdString();

  QDBusConnection bus = QDBusConnection::systemBus();
  QDBusInterface interface(dbus_helper::network_service(), pathCasted,
                           "org.freedesktop.DBus.Introspectable", bus);
  QDBusMessage message = interface.call("Introspect");
  checkForError(message);
  const auto check = message.arguments().first().toString().toStdString();
  return check.find("Wireless") != std::string::npos;
}
