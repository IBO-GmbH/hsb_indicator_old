#ifndef MOLD_SPROUT_VALUE_HPP
#define MOLD_SPROUT_VALUE_HPP

#include <chrono>
#include "mold/sprout_type.hpp"

namespace mold {

struct sprout_value {
  using timestamp = std::chrono::system_clock::time_point;

  sprout_type sprout_type_;
  timestamp timestamp_start;
  timestamp timestamp_change;
};

std::ostream& operator<<(std::ostream& out, const sprout_value& to_print);
}  // namespace mold

#endif  // MOLD_SPROUT_VALUE_HPP
