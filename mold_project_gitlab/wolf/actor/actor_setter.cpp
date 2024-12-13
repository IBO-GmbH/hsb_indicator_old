#include "actor_setter.hpp"

#include "actor_indexes.hpp"

using namespace logging;

wolf::actor::actor_setter::actor_setter() : m_logger("actor_setter") {}

void wolf::actor::actor_setter::set_actor(
    const controller_per_room_list::iterator &found, const unsigned int index,
    const bool state, std::stringstream &string) {
  if (!found->controller_->actors[index]) {
    string << std::dec << index;
    if (index < (wolf::controller::controller::actors_per_room - 1))
      string << ", ";
    return;
  }
  found->controller_->actors[index].get().state = state;
}

void wolf::actor::actor_setter::set_actors(
    const controller_per_room_list::iterator &found, const indexes &indexes,
    const bool state, std::stringstream &string) {
  for (unsigned int index = indexes.start; index <= indexes.end; ++index)
    set_actor(found, index, state, string);
}

bool wolf::actor::actor_setter::set_actors_svas2(
    const controller_per_room_list::iterator &found,
    const wolf::controller::svas2_dehumidifier::outputs &outputs,
    std::stringstream &string) {
  if (outputs.ventilation) {
    set_actor(found, ventilation_svas2, true, string);
    set_actor(found, dehumidifier_svas2, false, string);
    set_actors(found, {window_drive_1_svas2, window_drive_2_svas2}, true,
               string);
    return true;
  } else if (outputs.dehumidifier) {
    set_actor(found, ventilation_svas2, false, string);
    set_actor(found, dehumidifier_svas2, true, string);
    set_actors(found, {window_drive_1_svas2, window_drive_2_svas2}, false,
               string);
    return true;
  }
  return false;
}

bool wolf::actor::actor_setter::set_actors_svas3(
    const controller_per_room_list::iterator &found,
    const wolf::controller::svas3_heating::outputs &outputs,
    std::stringstream &string) {
  if (outputs.ventilation) {
    set_actor(found, ventilation_svas3, true, string);
    set_actor(found, heating_svas3, false, string);
    set_actors(found, {window_drive_1_svas3, window_drive_2_svas3}, true,
               string);
    return true;
  } else if (outputs.heating) {
    set_actor(found, ventilation_svas3, false, string);
    set_actor(found, heating_svas3, true, string);
    set_actors(found, {window_drive_1_svas3, window_drive_2_svas3}, false,
               string);
    return true;
  }
  return false;
}
