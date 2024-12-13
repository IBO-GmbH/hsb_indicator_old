#ifndef WIFI_ACCESS_POINT_GETTER_HPP
#define WIFI_ACCESS_POINT_GETTER_HPP

#include <memory>
#include "access_point.hpp"
#include "org_desktop_networkmanager_fwd.hpp"

namespace wifi {

class access_point_getter {
 public:
  access_point_getter(const std::string &path);
  ~access_point_getter();

  access_point get();

 private:
  std::unique_ptr<OrgFreedesktopNetworkManagerAccessPointInterface> m_interface;
};

}  // namespace wifi

#endif  // WIFI_ACCESS_POINT_GETTER_HPP
