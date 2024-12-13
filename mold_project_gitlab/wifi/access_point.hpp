#ifndef WIFI_ACCESS_POINT_HPP
#define WIFI_ACCESS_POINT_HPP

#include <ostream>

namespace wifi {

struct access_point {
  std::string path;
  std::string ssid;
  int strength;
  bool needs_password;
};

std::ostream& operator<<(std::ostream& out, const access_point& print);

}  // namespace wifi

#endif  // WIFI_ACCESS_POINT_HPP
