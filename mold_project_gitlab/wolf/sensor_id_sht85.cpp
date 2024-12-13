#include "sensor_id_sht85.hpp"
#include <cassert>

wolf::sensor_id wolf::sensor_id_sht85::create(
    const wolf::sensor_sht85_channel &sht85_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::sht85);
  id[1] = static_cast<std::uint8_t>(sht85_channel);
  const auto sensor_value_type = [&] {
    switch (sht85_channel) {
      case sensor_sht85_channel::humidity:
        return sensor_value_type::humidity;
      case sensor_sht85_channel::temperature:
        return sensor_value_type::temperature;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
