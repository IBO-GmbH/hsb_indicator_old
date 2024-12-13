#ifndef WOLF_SENSOR_ID_SHTC1_HPP
#define WOLF_SENSOR_ID_SHTC1_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_shtc1 {
 public:
  static sensor_id create(const sensor_shtc1_channel& shtc1_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_SHTC1_HPP
