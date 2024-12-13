#ifndef WOLF_TIMESTAMP_HELPER_HPP
#define WOLF_TIMESTAMP_HELPER_HPP

#include <chrono>

namespace wolf {

class timestamp_helper {
 public:
  timestamp_helper() = default;

  std::uint64_t cast_to_unix_seconds(
      const std::chrono::system_clock::time_point &to_cast) const;
  std::chrono::system_clock::time_point cast_seconds_to_chrono(
      std::uint64_t timestamp) const;
};

}  // namespace wolf

#endif
