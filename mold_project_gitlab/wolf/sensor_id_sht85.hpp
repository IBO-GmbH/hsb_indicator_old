#ifndef WOLF_SENSOR_ID_SHT85_HPP
#define WOLF_SENSOR_ID_SHT85_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_sht85 {
 public:
  static sensor_id create(const sensor_sht85_channel& sht85_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_SHT85_HPP
