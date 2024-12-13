#include "latest_value_helper.hpp"

using namespace mold::sprout;

mold::sprout_value latest_value_helper::operator()() const {
  const auto type_as_index = static_cast<std::size_t>(value.type_);
  const auto &result_timestamps = value.timestamps_[type_as_index];
  BOOST_ASSERT(result_timestamps.start);
  BOOST_ASSERT(result_timestamps.change);
  const auto timestamp_start = result_timestamps.start.get();
  const auto timestamp_change = result_timestamps.change.get();
  return sprout_value{value.type_, timestamp_start, timestamp_change};
}
