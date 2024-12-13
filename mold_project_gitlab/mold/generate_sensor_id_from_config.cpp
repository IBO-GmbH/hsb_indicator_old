#include "generate_sensor_id_from_config.hpp"

wolf::types::id mold::generate_sensor_id_from_config::generate(
    const wolf::types::uuid_array &config_id) {
  wolf::types::id sensor_id;
  sensor_id.reserve(config_id.size());
  std::copy(config_id.begin(), config_id.end(), std::back_inserter(sensor_id));
  return sensor_id;
}
