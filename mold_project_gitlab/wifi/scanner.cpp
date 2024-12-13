#include "scanner.hpp"
#include "access_point_getter.hpp"
#include "dbus_helper.hpp"
#include "org_desktop_networkmanager_device_accesspoint.h"
#include "org_desktop_networkmanager_device_wireless.h"

using namespace wifi;

static std::unique_ptr<OrgFreedesktopNetworkManagerDeviceWirelessInterface>
create_network_manager_device_wireless_interface(const std::string &path) {
  std::unique_ptr<OrgFreedesktopNetworkManagerDeviceWirelessInterface> result(
      new OrgFreedesktopNetworkManagerDeviceWirelessInterface(
          dbus_helper::network_service(), QString::fromStdString(path),
          QDBusConnection::systemBus()));
  return result;
}

scanner::scanner(const std::string &path, QObject *parent)
    : QObject(parent),
      m_logger{"scanner"},
      m_interface(create_network_manager_device_wireless_interface(path)) {
  read_access_points();
  connect(
      m_interface.get(),
      &OrgFreedesktopNetworkManagerDeviceWirelessInterface::AccessPointAdded,
      this, &scanner::handle_access_point_added);
  connect(
      m_interface.get(),
      &OrgFreedesktopNetworkManagerDeviceWirelessInterface::AccessPointRemoved,
      this, &scanner::handle_access_point_removed);
}

const scanner::access_points &scanner::get_access_points() const {
  return m_access_points;
}

void scanner::handle_access_point_added(const QDBusObjectPath &path) {
  const access_point to_add = read_access_point(path.path());
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "added access point " << to_add;
  m_access_points.push_back(to_add);
  signal_added(to_add);
}

void scanner::handle_access_point_removed(const QDBusObjectPath &path) {
  const auto path_casted = path.path().toStdString();
  const auto found = std::find_if(
      m_access_points.begin(), m_access_points.end(),
      [&](const access_point &check) { return check.path == path_casted; });
  if (found == m_access_points.cend()) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "remove. Could not find access point: '" << path_casted
        << "' in list";
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "removed access point " << *found;
  m_access_points.erase(found);
  signal_removed(path_casted);
}

void scanner::read_access_points() {
  QDBusPendingReply<QList<QDBusObjectPath> > reply =
      m_interface->GetAccessPoints();

  QList<QDBusObjectPath> value = reply.value();
  std::transform(value.cbegin(), value.cend(),
                 std::back_inserter(m_access_points),
                 [this](const QDBusObjectPath &path) {
                   return read_access_point(path.path());
                 });
}

access_point scanner::read_access_point(const QString &path) {
  access_point_getter getter(path.toStdString());
  return getter.get();
}

scanner::~scanner() = default;
