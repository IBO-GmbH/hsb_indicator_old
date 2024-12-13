#ifndef MOLD_CONFIGURATION_VALUES_OUTDOOR_HPP
#define MOLD_CONFIGURATION_VALUES_OUTDOOR_HPP

#include <boost/optional.hpp>

#include "mold/configuration_values.hpp"

namespace mold {

struct configuration_values_outdoor {
  boost::optional<configuration_values::value> temperature;
  boost::optional<configuration_values::value> humidity;
};

std::ostream& operator<<(std::ostream& out,
                         const configuration_values_outdoor& values);
}

#endif  // MOLD_CONFIGURATION_VALUES_OUTDOOR_HPP
