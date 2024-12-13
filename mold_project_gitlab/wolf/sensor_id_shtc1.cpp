#include "sensor_id_shtc1.hpp"
#include <cassert>

using namespace wolf;

sensor_id sensor_id_shtc1::create(const sensor_shtc1_channel &shtc1_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::shtc1);
  id[1] = static_cast<std::uint8_t>(shtc1_channel);
  const auto sensor_value_type = [&] {
    switch (shtc1_channel) {
      case sensor_shtc1_channel::humidity:
        return sensor_value_type::humidity;
      case sensor_shtc1_channel::temperature:
        return sensor_value_type::temperature;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
