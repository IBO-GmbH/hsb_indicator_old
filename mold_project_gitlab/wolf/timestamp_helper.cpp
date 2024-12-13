#include "timestamp_helper.hpp"

using namespace wolf;

uint64_t timestamp_helper::cast_to_unix_seconds(
    const std::chrono::system_clock::time_point &to_cast) const {
  return static_cast<const std::uint64_t>(
      std::chrono::duration_cast<std::chrono::seconds>(
          to_cast.time_since_epoch())
          .count());
}

std::chrono::system_clock::time_point timestamp_helper::cast_seconds_to_chrono(
    uint64_t timestamp) const {
  return std::chrono::system_clock::time_point(std::chrono::seconds(timestamp));
}
