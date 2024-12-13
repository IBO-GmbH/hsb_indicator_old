#include "sensor_id_evaluation_humidity.hpp"
#include "mold/sensor_id_from_uuid_and_types.hpp"

using namespace mold;

wolf::sensor_id sensor_id_evaluation_humidity::create(
    const wolf::types::uuid_array &config_id) {
  return sensor_id_from_uuid_and_types::create(
      config_id, wolf::sensor_type::evaluation_humidity,
      wolf::sensor_value_type::evaluation_humidity);
}
