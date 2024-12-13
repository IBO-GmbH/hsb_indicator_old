#ifndef MOLD_SPROUT_PERCENTAGE_CALCULATOR_HPP
#define MOLD_SPROUT_PERCENTAGE_CALCULATOR_HPP

#include "mold/sprout/data_types.hpp"

namespace mold {

class sprout_percentage_calculator {
  using time_point = std::chrono::system_clock::time_point;

 public:
  static float rising(const time_point& start, const time_point& now,
                      const sprout_type& type);
  static float falling(const sprout::data_types::timestamp& timestamps,
                       const time_point& now, const sprout_type& type);
  static time_point new_start(const sprout::data_types::timestamp& timestamps,
                              const time_point& now, const sprout_type& type);
  static sprout_type get_type_falling(
      const sprout::data_types::timestamps& timestamps);
  static bool all_equal(const sprout::data_types::timestamps& timestamps);

 private:
  static float get_percentage(const time_point& start, const time_point& now,
                              const sprout_type& type);
  static std::chrono::hours get_limit(const sprout_type& type);
  static bool all_start_equal(const sprout::data_types::timestamps& timestamps);
  static bool all_change_equal(
      const sprout::data_types::timestamps& timestamps);
};
}  // namespace mold

#endif  // MOLD_SPROUT_PERCENTAGE_CALCULATOR_HPP
