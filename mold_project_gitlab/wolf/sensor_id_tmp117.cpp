#include "sensor_id_tmp117.hpp"
#include <cassert>

wolf::sensor_id wolf::sensor_id_tmp117::create(
    const sensor_tmp117_channel& tmp117_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::tmp117);
  id[1] = static_cast<std::uint8_t>(tmp117_channel);
  const auto sensor_value_type = [&] {
    switch (tmp117_channel) {
      case sensor_tmp117_channel::temperature:
        return sensor_value_type::temperature;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
