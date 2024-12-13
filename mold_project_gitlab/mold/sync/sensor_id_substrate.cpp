#include "sensor_id_substrate.hpp"
#include "mold/sensor_id_from_uuid_and_types.hpp"

using namespace mold::sync;

wolf::sensor_id sensor_id_substrate::create(
    const wolf::types::uuid_array &config_id) {
  return sensor_id_from_uuid_and_types::create(
      config_id, wolf::sensor_type::substrate,
      wolf::sensor_value_type::substrate);
}
