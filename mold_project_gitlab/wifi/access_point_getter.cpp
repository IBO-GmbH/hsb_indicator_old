#include "access_point_getter.hpp"
#include <QDBusConnection>
#include "dbus_helper.hpp"
#include "org_desktop_networkmanager_device_accesspoint.h"

using namespace wifi;

static std::unique_ptr<OrgFreedesktopNetworkManagerAccessPointInterface>
create_interface(const QString &path) {
  std::unique_ptr<OrgFreedesktopNetworkManagerAccessPointInterface> result(
      new OrgFreedesktopNetworkManagerAccessPointInterface(
          dbus_helper::network_service(), path, QDBusConnection::systemBus()));
  return result;
}

access_point_getter::access_point_getter(const std::string &path)
    : m_interface(create_interface(QString::fromStdString(path))) {}

access_point_getter::~access_point_getter() = default;

access_point access_point_getter::get() {
  access_point result;
  result.path = m_interface->path().toStdString();
  result.ssid = QString::fromUtf8(m_interface->ssid()).toStdString();
  result.strength = m_interface->strength();
  // https://developer.gnome.org/NetworkManager/stable/gdbus-org.freedesktop.NetworkManager.AccessPoint.html
  // https://developer.gnome.org/NetworkManager/stable/nm-dbus-types.html#NM80211ApFlags
  result.needs_password = m_interface->flags() != 0;
  return result;
}
