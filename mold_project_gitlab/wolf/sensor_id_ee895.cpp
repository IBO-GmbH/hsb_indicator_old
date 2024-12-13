#include "sensor_id_ee895.hpp"
#include <cassert>

wolf::sensor_id wolf::sensor_id_ee895::create(
    const wolf::sensor_ee895_channel &ee895_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::ee895);
  id[1] = static_cast<std::uint8_t>(ee895_channel);
  const auto sensor_value_type = [&] {
    switch (ee895_channel) {
      case sensor_ee895_channel::co2:
        return sensor_value_type::co2;
      case sensor_ee895_channel::temperature:
        return sensor_value_type::temperature;
      case sensor_ee895_channel::air_pressure:
        return sensor_value_type::air_pressure;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
