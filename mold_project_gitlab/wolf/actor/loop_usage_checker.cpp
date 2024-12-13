#include "loop_usage_checker.hpp"

bool wolf::actor::loop_usage_checker::used_in_other_loops(
    const actor_array &all_actors, const actor &actor_to_check,
    const int check) {
  auto count = std::count_if(all_actors.cbegin(), all_actors.cend(),
                             [&actor_to_check](const optional_actor &to_check) {
                               if (!to_check) return false;
                               return to_check.get().id == actor_to_check.id;
                             });
  if (count > check) return true;
  return false;
}

wolf::actor::loop_usage_checker::actor_list_pointer
wolf::actor::loop_usage_checker::get_actors_of_loop(
    wolf::actor::loop_usage_checker::actor_array &all_actors,
    const unsigned int start, const unsigned int end) {
  actor_list_pointer actors_in_this_loop;
  for (unsigned int index = start; index <= end; ++index)
    actors_in_this_loop.push_back(&all_actors[index]);
  return actors_in_this_loop;
}

bool wolf::actor::loop_usage_checker::used_in_other_rooms(
    const wolf::actor::loop_usage_checker::controller_list &controller_list_,
    const types::uuid_array &room_id,
    const wolf::actor::actor &actor_to_check) {
  for (const auto &controller : controller_list_) {
    if (room_id == controller.room_id) continue;
    auto &all_actors = controller.controller_->actors;
    if (used_in_other_loops(all_actors, actor_to_check, 0)) return true;
  }
  return false;
}
