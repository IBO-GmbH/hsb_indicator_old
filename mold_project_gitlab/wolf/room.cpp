#include "room.hpp"

#include <iterator>

using namespace wolf;

std::ostream &wolf::operator<<(std::ostream &os, const room &to_render) {
  os << "{id:" << to_render.id << ", name:" << to_render.name << ", sensors:{";
  std::copy(to_render.sensors.cbegin(), to_render.sensors.cend(),
            std::ostream_iterator<sensor_id>(os, ", "));
  return os << "}}";
}

bool wolf::room::operator==(const wolf::room &compare) const {
  return compare.id == id && compare.name == name && compare.sensors == sensors;
}

bool room::operator!=(const room &compare) const { return !(*this == compare); }
