#include "sensor_id_mqtt.hpp"

wolf::sensor_id mold::sensor_id_mqtt::create(
    const std::string &client_id, const wolf::sensor_value_type &value_type) {
  wolf::types::id id(client_id.begin(), client_id.end());
  id.insert(id.begin(), static_cast<std::uint8_t>(wolf::sensor_type::mqtt));
  id.push_back(static_cast<std::uint8_t>(value_type));
  return wolf::sensor_id{id, value_type};
}
