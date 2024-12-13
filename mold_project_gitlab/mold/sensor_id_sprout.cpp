#include "sensor_id_sprout.hpp"
#include "mold/sensor_id_from_uuid_and_types.hpp"

wolf::sensor_id mold::sensor_id_sprout::create(
    const wolf::types::uuid_array &config_id) {
  return sensor_id_from_uuid_and_types::create(config_id,
                                               wolf::sensor_type::sprout_value,
                                               wolf::sensor_value_type::sprout);
}
