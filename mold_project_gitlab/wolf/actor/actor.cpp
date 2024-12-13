#include "actor.hpp"

#include <ostream>

std::ostream &wolf::actor::operator<<(std::ostream &out,
                                      const wolf::actor::actor &to_print) {
  out << "{actor_id: " << std::dec << to_print.id
      << ", actor_state: " << to_print.state;
  return out << "}";
}

bool wolf::actor::operator==(const wolf::actor::actor &first,
                             const wolf::actor::actor &second) {
  return first.id == second.id && first.state == second.state;
}

bool wolf::actor::operator!=(const wolf::actor::actor &first,
                             const wolf::actor::actor &second) {
  return !(first == second);
}
