#include "active_access_point_getter.hpp"
#include <QDBusConnection>
#include "dbus_helper.hpp"
#include "org_desktop_networkmanager_device_wireless.h"

using namespace wifi;

static std::unique_ptr<OrgFreedesktopNetworkManagerDeviceWirelessInterface>
create_network_manager_device_wireless_interface(const std::string &path) {
  auto result =
      std::make_unique<OrgFreedesktopNetworkManagerDeviceWirelessInterface>(
          dbus_helper::network_service(), QString::fromStdString(path),
          QDBusConnection::systemBus());
  return result;
}

active_access_point_getter::active_access_point_getter(const std::string &path,
                                                       QObject *parent)
    : QObject(parent),
      m_logger{"active_access_point_getter"},
      m_interface(create_network_manager_device_wireless_interface(path)) {}

active_access_point_getter::~active_access_point_getter() = default;

boost::optional<std::string> active_access_point_getter::get() {
  const QString path = m_interface->activeAccessPoint().path();
  if (path.isEmpty() || path == "/") return {};
  return path.toStdString();
}
