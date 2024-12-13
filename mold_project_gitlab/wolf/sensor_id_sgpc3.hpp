#ifndef WOLF_SENSOR_ID_SGPC3_HPP
#define WOLF_SENSOR_ID_SGPC3_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_sgpc3 {
 public:
  static sensor_id create(const sensor_sgpc3_channel& sgpc3_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_SGPC3_HPP
