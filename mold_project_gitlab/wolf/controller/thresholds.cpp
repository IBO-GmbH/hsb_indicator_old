#include "thresholds.hpp"

std::ostream &wolf::controller::operator<<(
    std::ostream &out, const wolf::controller::thresholds &to_print) {
  out << "{upper_threshold: " << std::dec << to_print.upper
      << ", lower_threshold: " << std::dec << to_print.lower;
  return out << "}";
}
