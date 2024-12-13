#ifndef MOLD_CALCULATION_STATE_HPP
#define MOLD_CALCULATION_STATE_HPP

#include <ostream>

namespace mold {

enum struct calculation_state : int {
  // DON'T change order. affects sensor_id serialisation.
  green = 0,
  yellow = 1,
  yellow_confirmed = 2,
  red = 3,
  red_confirmed = 4,
  undefined = 5
};

inline bool calculation_state_is_yellow(const calculation_state& state) {
  return state == calculation_state::yellow ||
         state == calculation_state::yellow_confirmed;
}
inline bool calculation_state_is_red(const calculation_state& state) {
  return state == calculation_state::red ||
         state == calculation_state::red_confirmed;
}

std::ostream& operator<<(std::ostream& out, const calculation_state& to_print);
}  // namespace mold

#endif
