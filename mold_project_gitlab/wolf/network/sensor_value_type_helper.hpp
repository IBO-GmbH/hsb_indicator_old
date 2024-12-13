#ifndef WOLF_NETWORK_SENSOR_TYPE_HELPER_HPP
#define WOLF_NETWORK_SENSOR_TYPE_HELPER_HPP

#include "mold_protocol_types.h"
#include "wolf/sensor_id.hpp"

namespace wolf::network {

class sensor_value_type_helper {
 public:
  sensor_value_type_helper() = default;

  Clima::Enums::SensorType cast_to_network(const sensor_id& to_cast);
  sensor_value_type cast_to_wolf(const Clima::Enums::SensorType to_cast);
};

}  // namespace wolf::network

#endif
