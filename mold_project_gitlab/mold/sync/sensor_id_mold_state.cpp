#include "sensor_id_mold_state.hpp"
#include "mold/sensor_id_from_uuid_and_types.hpp"

using namespace mold::sync;

wolf::sensor_id sensor_id_mold_state::create(
    const wolf::types::uuid_array &config_id) {
  return sensor_id_from_uuid_and_types::create(
      config_id, wolf::sensor_type::mold_state,
      wolf::sensor_value_type::mold_state);
}
