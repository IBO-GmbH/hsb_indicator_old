#include "device_state_watcher.hpp"
#include "dbus_helper.hpp"
#include "org_desktop_networkmanager_device.h"

using namespace wifi;

static std::unique_ptr<OrgFreedesktopNetworkManagerDeviceInterface>
create_network_manager(const std::string& device) {
  std::unique_ptr<OrgFreedesktopNetworkManagerDeviceInterface> result(
      new OrgFreedesktopNetworkManagerDeviceInterface(
          dbus_helper::network_service(), QString::fromStdString(device),
          QDBusConnection::systemBus()));
  return result;
}

device_state_watcher::device_state_watcher(const std::string& path,
                                           QObject* parent)
    : QObject(parent),
      m_logger{"device_state_watcher"},
      m_interface(create_network_manager(path)) {
  m_state = static_cast<device_state>(m_interface->state());
  connect(m_interface.get(),
          &OrgFreedesktopNetworkManagerDeviceInterface::StateChanged, this,
          &device_state_watcher::handle_state_changed);
}

device_state device_state_watcher::get_state() const { return m_state; }

void device_state_watcher::handle_state_changed(uint new_state,
                                                uint /*old_state*/,
                                                uint /*reason*/) {
  const auto result = static_cast<device_state>(new_state);
  m_state = result;
  LOGGING_LOG(m_logger, logging ::severity::verbose)
      << "device state changed: " << static_cast<int>(result);
  signal_state_changed(result);
}

device_state_watcher::~device_state_watcher() = default;
