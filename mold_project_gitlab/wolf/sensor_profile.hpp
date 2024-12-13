#ifndef WOLF_SENSOR_PROFILE_HPP
#define WOLF_SENSOR_PROFILE_HPP

#include <cmath>
#include <limits>
#include "wolf/sensor_id.hpp"
#include "wolf/sensor_metadata.hpp"

namespace wolf {

struct sensor_profile {
  sensor_id id;
  sensor_type type;
  sensor_metadata metadata;

  inline bool operator==(const sensor_profile& other) const;
  inline bool operator!=(const sensor_profile& other) const;
};

bool sensor_profile::operator==(const sensor_profile& other) const {
  return id == other.id && type == other.type && metadata == other.metadata;
}

bool sensor_profile::operator!=(const sensor_profile& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const sensor_profile& out);
std::ostream& operator<<(std::ostream& out,
                         const std::vector<sensor_profile>& to_print);
}  // namespace wolf

#endif  // WOLF_SENSOR_PROFILE_HPP
