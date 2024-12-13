#include "reset_mode.hpp"

#include <cassert>

std::ostream &mold::operator<<(std::ostream &os, const reset_mode &out) {
  switch (out) {
    case reset_mode::shutdown:
      return os << "shutdown";
    case reset_mode::reboot:
      return os << "reboot";
  }
  assert(false);
  return os << "undefined";
}
