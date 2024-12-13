#ifndef WOLF_ROOM_HPP
#define WOLF_ROOM_HPP

#include "wolf/sensor_id.hpp"

namespace wolf {

struct room {
  types::uuid_array id;
  std::string name;
  std::vector<sensor_id> sensors;

  bool operator==(const room& compare) const;
  bool operator!=(const room& compare) const;
};

std::ostream& operator<<(std::ostream& os, const room& to_render);
}

#endif  // WOLF_ROOM_HPP
