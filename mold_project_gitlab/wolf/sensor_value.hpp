#ifndef WOLF_SENSOR_VALUE_HPP
#define WOLF_SENSOR_VALUE_HPP

#include <chrono>
#include "sensor_id.hpp"

namespace wolf {

struct sensor_value {
  using time_point = std::chrono::system_clock::time_point;
  sensor_id id;
  float value;
  time_point timestamp;
};
using sensor_values = std::vector<sensor_value>;

std::ostream& operator<<(std::ostream& out, const sensor_value& print);
std::ostream& operator<<(std::ostream& out, const sensor_values& to_print);

}  // namespace wolf

#endif  // WOLF_SENSOR_VALUE_HPP
