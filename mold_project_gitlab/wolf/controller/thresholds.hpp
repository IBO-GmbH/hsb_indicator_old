#ifndef WOLF_CONTROLLER_THRESHOLDS_HPP
#define WOLF_CONTROLLER_THRESHOLDS_HPP

#include <ostream>

namespace wolf {

namespace controller {

struct thresholds {
  float upper;
  float lower;
};

std::ostream& operator<<(std::ostream& out, const thresholds& to_print);
}
}
#endif  // WOLF_CONTROLLER_THRESHOLDS_HPP
