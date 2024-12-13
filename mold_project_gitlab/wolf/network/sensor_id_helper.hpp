#ifndef WOLF_NETWORK_SENSOR_ID_HELPER_HPP
#define WOLF_NETWORK_SENSOR_ID_HELPER_HPP

#include "wolf/mold_protocol_types.h"
#include "wolf/sensor_id.hpp"

namespace wolf::network {

class sensor_id_helper {
 public:
  sensor_id_helper() = default;

  Clima::SensorId cast_to_network(const sensor_id& to_cast);
  sensor_id cast_from_network(const Clima::SensorId& to_cast,
                              const wolf::sensor_value_type type);
};
}  // namespace wolf::network

#endif
