#include "configuration_values_outdoor.hpp"

#include <boost/optional/optional_io.hpp>

std::ostream &mold::operator<<(
    std::ostream &out, const mold::configuration_values_outdoor &values) {
  return out << "{temperature:" << values.temperature
             << ", humidity:" << values.humidity << "}";
}
