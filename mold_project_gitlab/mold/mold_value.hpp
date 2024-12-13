#ifndef MOLD_VALUE_HPP
#define MOLD_VALUE_HPP

#include <chrono>
#include "mold/sprout_type.hpp"
#include "wolf/types.hpp"

namespace mold {
struct mold_value {
  using time_point = std::chrono::system_clock::time_point;

  wolf::types::uuid_array configuration;
  float percentage;
  sprout_type sprout_type_;
  time_point timestamp;
};
}  // namespace mold

#endif  // MOLD_VALUE_HPP
