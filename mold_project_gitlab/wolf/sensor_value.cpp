#include "sensor_value.hpp"
#include <experimental/iterator>
#include "wolf/timestamp_helper.hpp"

std::ostream& wolf::operator<<(std::ostream& out, const sensor_value& print) {
  wolf::timestamp_helper helper;
  return out << "{id:" << print.id << ", value:" << print.value
             << ", timestamp:" << std::dec
             << helper.cast_to_unix_seconds(print.timestamp) << "}";
}

std::ostream& wolf::operator<<(std::ostream& out,
                               const wolf::sensor_values& to_print) {
  out << "[";
  std::copy(to_print.cbegin(), to_print.cend(),
            std::experimental::make_ostream_joiner(out, ", "));
  return out << "]";
}
