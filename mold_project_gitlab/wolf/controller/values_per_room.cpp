#include "values_per_room.hpp"

std::ostream &wolf::controller::operator<<(
    std::ostream &out, const wolf::controller::values_per_room &to_print) {
  out << "{room_id: " << to_print.room_id << ", limits: " << to_print.limits_
      << ", actors: {";
  for (auto iterator = to_print.actors.begin();
       iterator != to_print.actors.end(); iterator++) {
    if (iterator != to_print.actors.begin()) out << ", ";
    out << *iterator;
  }
  out << "}, active states: {";
  for (auto iterator = to_print.active_loops.begin();
       iterator != to_print.active_loops.end(); iterator++) {
    if (iterator != to_print.active_loops.begin()) out << ", ";
    out << *iterator;
  }
  return out << "}}";
}

bool wolf::controller::operator==(
    const wolf::controller::values_per_room &first,
    const wolf::controller::values_per_room &second) {
  return first.room_id == second.room_id && first.limits_ == second.limits_ &&
         first.actors == second.actors &&
         first.active_loops == second.active_loops;
}

bool wolf::controller::operator!=(
    const wolf::controller::values_per_room &first,
    const wolf::controller::values_per_room &second) {
  return !(first == second);
}
