#ifndef WOLF_SENSOR_ID_TMP117_HPP
#define WOLF_SENSOR_ID_TMP117_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_tmp117 {
 public:
  static sensor_id create(const sensor_tmp117_channel& tmp117_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_TMP117_HPP
