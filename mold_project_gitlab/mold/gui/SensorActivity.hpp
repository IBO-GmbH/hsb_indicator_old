#ifndef MOLD_SENSORACTIVITY_HPP
#define MOLD_SENSORACTIVITY_HPP

#include "wolf/sensor_id.hpp"

#include <chrono>

namespace mold {

struct SensorActivity {
  using time_point = std::chrono::system_clock::time_point;

  std::string room_name;
  int rssi;
  time_point time;
};

std::ostream& operator<<(std::ostream& out, const SensorActivity& to_print);
}  // namespace mold

#endif  // MOLD_SENSORACTIVITY_HPP
