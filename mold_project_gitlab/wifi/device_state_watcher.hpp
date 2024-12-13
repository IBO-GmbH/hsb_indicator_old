#ifndef WIFI_DEVICE_STATE_WATCHER_HPP
#define WIFI_DEVICE_STATE_WATCHER_HPP

#include <QObject>
#include <boost/signals2/signal.hpp>
#include "device_state.hpp"
#include "log/logger.hpp"
#include "org_desktop_networkmanager_fwd.hpp"

namespace wifi {

class device_state_watcher : public QObject {
  Q_OBJECT
 public:
  device_state_watcher(const std::string& path, QObject* parent = nullptr);
  ~device_state_watcher();

  device_state get_state() const;
  boost::signals2::signal<void(device_state)> signal_state_changed;

 private:
  void handle_state_changed(uint new_state, uint old_state, uint reason);

  logging::logger m_logger;
  std::unique_ptr<OrgFreedesktopNetworkManagerDeviceInterface> m_interface;
  device_state m_state;
};

}  // namespace wifi

#endif  // WIFI_DEVICE_STATE_WATCHER_HPP
