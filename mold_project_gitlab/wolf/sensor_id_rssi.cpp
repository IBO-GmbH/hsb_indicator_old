#include "sensor_id_rssi.hpp"
#include <algorithm>
#include "sensor_id_enocean.hpp"

wolf::sensor_id wolf::sensor_id_rssi::create_from_esp3_id(
    const wolf::types::id_esp3 &esp3_id) {
  types::id id(5);
  id[0] = static_cast<std::uint8_t>(sensor_type::sensor_rssi);
  std::copy_n(reinterpret_cast<const std::uint8_t *>(&esp3_id), 4,
              std::next(id.begin(), 1));
  return sensor_id(id, sensor_value_type::sensor_rssi);
}

wolf::sensor_id wolf::sensor_id_rssi::create_from_sensor_id(
    const wolf::sensor_id &sensor_id_value) {
  const auto esp3_id = sensor_id_enocean::convert_to_esp3_id(sensor_id_value);
  return create_from_esp3_id(esp3_id);
}
