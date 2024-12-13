#include "sensor_id_sht25.hpp"
#include <cassert>

using namespace wolf;

sensor_id sensor_id_sht25::create(const sensor_sht25_channel &sht25_channel) {
  types::id id(2);
  id.front() = static_cast<std::uint8_t>(sensor_type::sht25);
  id[1] = static_cast<std::uint8_t>(sht25_channel);
  const auto sensor_value_type = [&] {
    switch (sht25_channel) {
      case sensor_sht25_channel::humidity:
        return sensor_value_type::humidity;
      case sensor_sht25_channel::temperature:
        return sensor_value_type::temperature;
    }
    assert(false);
    return sensor_value_type::unknown;
  }();
  return sensor_id{id, sensor_value_type};
}
