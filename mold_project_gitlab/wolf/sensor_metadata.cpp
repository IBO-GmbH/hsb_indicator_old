#include "sensor_metadata.hpp"
#include <iostream>
#include "wolf/math_compare_decimals.hpp"

std::ostream &wolf::operator<<(std::ostream &os,
                               const wolf::sensor_metadata &out) {
  os << "{type: " << out.type << ", min_value: " << out.min_value
     << ", max_value: " << out.max_value << ", {";
  for (const auto &item : out.properties) {
    os << "property: " << item.first << ", value: " << item.second << ", ";
  }
  os << "}}";
  return os;
}

bool wolf::sensor_metadata::operator==(
    const wolf::sensor_metadata &other) const {
  return type == other.type &&
         math::compare_floats(min_value, other.min_value) &&
         math::compare_floats(max_value, other.max_value) &&
         properties == other.properties;
}

bool wolf::sensor_metadata::operator!=(
    const wolf::sensor_metadata &other) const {
  return !(*this == other);
}
