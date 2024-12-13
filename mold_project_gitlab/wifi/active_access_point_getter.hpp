#ifndef WIFI_ACTIVE_ACCESS_POINT_GETTER_HPP
#define WIFI_ACTIVE_ACCESS_POINT_GETTER_HPP

#include <QObject>
#include <boost/optional.hpp>
#include <memory>
#include "log/logger.hpp"
#include "org_desktop_networkmanager_fwd.hpp"

namespace wifi {

class active_access_point_getter : public QObject {
  Q_OBJECT
 public:
  active_access_point_getter(const std::string& path,
                             QObject* parent = nullptr);
  ~active_access_point_getter();

  boost::optional<std::string> get();

 private:
  logging::logger m_logger;
  std::unique_ptr<OrgFreedesktopNetworkManagerDeviceWirelessInterface>
      m_interface;
};

}  // namespace wifi

#endif  // WIFI_ACTIVE_ACCESS_POINT_GETTER_HPP
