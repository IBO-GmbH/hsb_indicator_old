#include "type.hpp"

#include <cassert>

std::ostream &wolf::controller::operator<<(std::ostream &os,
                                           const wolf::controller::type &out) {
  switch (out) {
    case wolf::controller::type::absolute_humidity:
      return os << "absolute_humidity";
    case wolf::controller::type::relative_humidity:
      return os << "relative_humidity";
    case wolf::controller::type::co2:
      return os << "co2";
    case wolf::controller::type::sprout_value:
      return os << "sprout_value";
    case wolf::controller::type::voc:
      return os << "voc";
  }
  assert(false);
  return os << "undefined";
}
