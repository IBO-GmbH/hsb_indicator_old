#ifndef WOLF_SENSOR_ID_RSSI_HPP
#define WOLF_SENSOR_ID_RSSI_HPP

#include "sensor_id.hpp"

namespace wolf {

class sensor_id_rssi {
 public:
  static sensor_id create_from_esp3_id(const types::id_esp3& esp3_id);
  static sensor_id create_from_sensor_id(const sensor_id& sensor_id_value);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_RSSI_HPP
