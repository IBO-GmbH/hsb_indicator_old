#ifndef MOLD_GENERATE_SENSOR_ID_FROM_CONFIG_HPP
#define MOLD_GENERATE_SENSOR_ID_FROM_CONFIG_HPP

#include "wolf/types.hpp"

namespace mold {

class generate_sensor_id_from_config {
 public:
  static wolf::types::id generate(const wolf::types::uuid_array &config_id);
};
}  // namespace mold

#endif  // MOLD_GENERATE_SENSOR_ID_FROM_CONFIG_HPP
