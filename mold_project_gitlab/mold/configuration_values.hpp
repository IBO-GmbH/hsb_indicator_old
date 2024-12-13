#ifndef MOLD_CONFIGURATION_VALUES_HPP
#define MOLD_CONFIGURATION_VALUES_HPP

#include <boost/optional.hpp>
#include <chrono>
#include <ostream>
#include "wolf/types.hpp"

namespace mold {

struct configuration_values {
  using timestamp = std::chrono::system_clock::time_point;

  // TODO use sensor_value?
  struct value {
    timestamp timestamp_;
    float value_;

    bool operator==(const value& compare) const;
    bool operator!=(const value& compare) const;
  };
  wolf::types::uuid_array configuration_id;
  boost::optional<value> indoor_temperature;
  boost::optional<value> indoor_humidity;

  bool operator==(const configuration_values& compare) const;
  bool operator!=(const configuration_values& compare) const;
};

std::ostream& operator<<(std::ostream& out,
                         const configuration_values::value& to_print);
std::ostream& operator<<(std::ostream& out,
                         const configuration_values& to_print);
}  // namespace mold

#endif  // MOLD_CONFIGURATION_VALUES_HPP
