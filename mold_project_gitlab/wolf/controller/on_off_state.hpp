#ifndef WOLF_CONTROLLER_ON_OFF_STATE_HPP
#define WOLF_CONTROLLER_ON_OFF_STATE_HPP

#include "thresholds.hpp"

namespace wolf {

namespace controller {

struct on_off_state {
  thresholds threshold;
  bool output;
};

std::ostream& operator<<(std::ostream& out, const on_off_state& to_print);
}
}
#endif  // WOLF_CONTROLLER_ON_OFF_STATE_HPP
