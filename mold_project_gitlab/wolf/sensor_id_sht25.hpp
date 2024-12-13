#ifndef WOLF_SENSOR_ID_SHT25_HPP
#define WOLF_SENSOR_ID_SHT25_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_sht25 {
 public:
  static sensor_id create(const sensor_sht25_channel& sht25_channel);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_SHT25_HPP
