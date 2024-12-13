#ifndef WOLF_FOREIGN_SENSOR_HPP
#define WOLF_FOREIGN_SENSOR_HPP

#include "wolf/sensor_profile.hpp"

namespace wolf {

struct foreign_sensor {
  wolf::sensor_profile profile;
  std::string description;
};

std::ostream& operator<<(std::ostream& out, const foreign_sensor& to_print);

}  // namespace wolf

#endif
