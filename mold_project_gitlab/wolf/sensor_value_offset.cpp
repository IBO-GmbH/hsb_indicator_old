#include "sensor_value_offset.hpp"

std::ostream &wolf::operator<<(std::ostream &out,
                               const wolf::sensor_value_offset &print) {
  return out << "{id:" << print.id << ", offset:" << print.offset << "}";
}
