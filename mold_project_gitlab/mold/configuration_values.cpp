#include "configuration_values.hpp"
#include <boost/optional/optional_io.hpp>

using namespace mold;

bool configuration_values::operator==(
    const configuration_values &compare) const {
  return compare.configuration_id == configuration_id &&
         compare.indoor_humidity == indoor_humidity &&
         compare.indoor_temperature == indoor_temperature;
}

bool configuration_values::operator!=(
    const configuration_values &compare) const {
  return !(*this == compare);
}

bool configuration_values::value::operator==(
    const configuration_values::value &compare) const {
  return std::abs(compare.value_ - value_) <=
             std::numeric_limits<float>::epsilon() &&
         compare.timestamp_ == timestamp_;
}

bool configuration_values::value::operator!=(
    const configuration_values::value &compare) const {
  return !(*this == compare);
}

std::ostream &mold::operator<<(std::ostream &out,
                               const configuration_values &to_print) {
  return out << "{indoor_temperature:" << to_print.indoor_temperature
             << " indoor_humidity:" << to_print.indoor_humidity << "}";
}

std::ostream &mold::operator<<(std::ostream &out,
                               const configuration_values::value &to_print) {
  return out << "{value:" << to_print.value_ << " timestamp:" << std::dec
             << std::chrono::duration_cast<std::chrono::seconds>(
                    to_print.timestamp_.time_since_epoch())
                    .count()
             << "}";
}
