#include "controller.hpp"

#include "type.hpp"

std::ostream &wolf::controller::operator<<(std::ostream &out,
                                           const controller &to_print) {
  out << "{controllers: ";
  for (unsigned int counter = 0; counter < controller::different_types;
       ++counter) {
    out << "{type: " << static_cast<type>(counter)
        << ", on_off_control: " << to_print.controller_[counter] << "}, ";
  }
  out << "actors: ";
  for (unsigned int counter = 0; counter < controller::actors_per_room;
       ++counter) {
    if (to_print.actors[counter])
      out << to_print.actors[counter].get();
    else
      out << "{not set}";
    if (counter != controller::actors_per_room - 1) out << ", ";
  }
  return out << "}";
}
