#ifndef WOLF_OUTDOOR_SENSOR_HPP
#define WOLF_OUTDOOR_SENSOR_HPP

#include "sensor_id.hpp"

namespace wolf {

struct outdoor_sensor {
  outdoor_sensor() = default;
  explicit outdoor_sensor(const sensor_id& temperature,
                          const sensor_id& humidity);

  bool is_set() const;

  sensor_id temperature;
  sensor_id humidity;
};

bool operator==(const outdoor_sensor& first, const outdoor_sensor& second);
bool operator!=(const outdoor_sensor& first, const outdoor_sensor& second);

std::ostream& operator<<(std::ostream& out, const outdoor_sensor& to_print);
}

#endif  // WOLF_OUTDOOR_SENSOR_HPP
