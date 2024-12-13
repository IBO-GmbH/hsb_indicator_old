#ifndef MOLD_SPROUT_DATA_TYPES_HPP
#define MOLD_SPROUT_DATA_TYPES_HPP

#include <boost/optional.hpp>
#include <chrono>
#include "mold/sprout_type.hpp"
#include "wolf/types.hpp"

namespace mold::sprout::data_types {
using optional_time_point =
    boost::optional<std::chrono::system_clock::time_point>;

struct timestamp {
  optional_time_point start;
  optional_time_point change;
};
using timestamps = std::array<timestamp, 6>;
struct value {
  wolf::types::uuid_array configuration;
  sprout_type type_{sprout_type::infinite_days};
  timestamps timestamps_;
};
using values = std::vector<value>;
}  // namespace mold::sprout::data_types

#endif
