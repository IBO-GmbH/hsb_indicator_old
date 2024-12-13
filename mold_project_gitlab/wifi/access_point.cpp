#include "access_point.hpp"

std::ostream &wifi::operator<<(std::ostream &out,
                               const wifi::access_point &print) {
  return out << "{path:" << print.path << ", ssid:" << print.ssid
             << ", strength:" << print.strength
             << ", needs_password:" << print.needs_password << "}";
}
