#include "on_off_state.hpp"

std::ostream &wolf::controller::operator<<(
    std::ostream &out, const wolf::controller::on_off_state &to_print) {
  out << "{thresholds: " << to_print.threshold
      << ", output: " << to_print.output;
  return out << "}";
}
