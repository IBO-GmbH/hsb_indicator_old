#ifndef WOLF_NETWORK_SENSOR_PROFILE_HELPER_HPP
#define WOLF_NETWORK_SENSOR_PROFILE_HELPER_HPP

#include "wolf/mold_protocol_types.h"
#include "wolf/sensor_profile.hpp"

namespace wolf::network {

class sensor_profile_helper {
 public:
  sensor_profile_helper() = default;

  sensor_profile cast_to_wolf(const Clima::SensorProfile& to_cast);
  Clima::SensorMetadata cast_to_network(const sensor_metadata& to_cast);
  Clima::SensorProfile cast_to_network(const sensor_profile& to_cast);
};

}  // namespace wolf::network

#endif
