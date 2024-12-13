#ifndef WOLF_CONTROLLER_TYPE_HPP
#define WOLF_CONTROLLER_TYPE_HPP

#include <ostream>

namespace wolf {

namespace controller {

enum struct type {
  // DON'T change order. affects sensor_id serialisation.
  absolute_humidity,
  relative_humidity,
  co2,
  sprout_value,
  voc
};

std::ostream& operator<<(std::ostream& os, const type& out);
}  // namespace controller
}  // namespace wolf
#endif  // WOLF_CONTROLLER_TYPE_HPP
