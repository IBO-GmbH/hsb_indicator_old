#ifndef WOLF_SENSOR_ID_EE895_HPP
#define WOLF_SENSOR_ID_EE895_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_ee895 {
 public:
  static sensor_id create(const sensor_ee895_channel& ee895_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_EE895_HPP
