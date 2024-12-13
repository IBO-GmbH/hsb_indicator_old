#include "sprout_value.hpp"

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::sprout_value &to_print) {
  out << "{sprout type: " << to_print.sprout_type_
      << ", timestamp start: " << std::dec
      << to_print.timestamp_start.time_since_epoch().count()
      << ", timestamp change: " << std::dec
      << to_print.timestamp_change.time_since_epoch().count();
  return out << "}";
}
