#ifndef WOLF_SENSOR_ID_SCD30_HPP
#define WOLF_SENSOR_ID_SCD30_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

class sensor_id_scd30 {
 public:
  static sensor_id create(const sensor_scd30_channel& scd30_channel);
};
}  // namespace wolf

#endif
