#include "sensor_id_scd30.hpp"
#include <cassert>

using namespace wolf;

sensor_id sensor_id_scd30::create(const sensor_scd30_channel &scd30_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::scd30);
  id[1] = static_cast<std::uint8_t>(scd30_channel);
  const auto sensor_value_type = [&] {
    switch (scd30_channel) {
      case sensor_scd30_channel::co2:
        return sensor_value_type::co2;
      case sensor_scd30_channel::humidity:
        return sensor_value_type::humidity;
      case sensor_scd30_channel::temperature:
        return sensor_value_type::temperature;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
