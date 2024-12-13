#include "sprout_type.hpp"

#include <cassert>

std::ostream &mold::operator<<(std::ostream &os, const mold::sprout_type &out) {
  switch (out) {
    case mold::sprout_type::one_day:
      return os << "one_day";
    case mold::sprout_type::two_days:
      return os << "two_days";
    case mold::sprout_type::four_days:
      return os << "four_days";
    case mold::sprout_type::eight_days:
      return os << "eight_days";
    case mold::sprout_type::sixteen_days:
      return os << "sixteen_days";
    case mold::sprout_type::infinite_days:
      return os << "infinite_days";
  }
  assert(false);
  return os << "undefined";
}
