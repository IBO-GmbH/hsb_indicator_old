#ifndef WOLF_SENSOR_VALUE_OFFSET_HPP
#define WOLF_SENSOR_VALUE_OFFSET_HPP

#include <ostream>
#include "wolf/sensor_id.hpp"

namespace wolf {

struct sensor_value_offset {
  sensor_id id;
  float offset;
};

std::ostream& operator<<(std::ostream& out,
                         const wolf::sensor_value_offset& print);

}  // namespace wolf

#endif
