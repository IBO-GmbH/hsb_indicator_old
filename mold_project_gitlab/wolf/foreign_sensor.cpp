#include "foreign_sensor.hpp"

std::ostream &wolf::operator<<(std::ostream &out,
                               const wolf::foreign_sensor &to_print) {
  return out << "{description:" << to_print.description
             << ", profile:" << to_print.profile << "}";
}
