#include "sensor_profile.hpp"
#include <experimental/iterator>

std::ostream &wolf::operator<<(std::ostream &os, const sensor_profile &out) {
  return os << "{id:" << out.id << ", type:" << out.type
            << ", metadata: " << out.metadata << "}";
}

std::ostream &wolf::operator<<(
    std::ostream &out, const std::vector<wolf::sensor_profile> &to_print) {
  out << "[";
  std::copy(to_print.cbegin(), to_print.cend(),
            std::experimental::make_ostream_joiner(out, ", "));
  return out << "]";
}
