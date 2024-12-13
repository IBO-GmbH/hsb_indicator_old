#ifndef WIFI_DEVICE_FINDER_HPP
#define WIFI_DEVICE_FINDER_HPP

#include <boost/optional/optional.hpp>
#include "log/logger.hpp"
#include "org_desktop_networkmanager_fwd.hpp"

class QDBusObjectPath;

namespace wifi {

class device_finder {
 public:
  device_finder();
  ~device_finder();

  boost::optional<std::string> get_first_wifi_device();

 private:
  void ensure_connected() const;
  bool is_device_wifi(const QDBusObjectPath& path);

  logging::logger m_logger;
  std::unique_ptr<OrgFreedesktopNetworkManagerInterface> m_networkmanager;
};

}  // namespace wifi

#endif  // WIFI_DEVICE_FINDER_HPP
