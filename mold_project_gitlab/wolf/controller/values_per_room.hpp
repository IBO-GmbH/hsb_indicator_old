#ifndef WOLF_CONTROLLER_VALUES_PER_ROOM_HPP
#define WOLF_CONTROLLER_VALUES_PER_ROOM_HPP

#include "controller.hpp"
#include "limits.hpp"
#include "wolf/types.hpp"

#include <chrono>

namespace wolf {

namespace controller {

struct values_per_room {
  using active_states = std::array<bool, controller::control_loops_per_room>;

  types::uuid_array room_id;
  limits limits_;
  controller::actor_array actors;
  active_states active_loops;
};

bool operator==(const values_per_room& first, const values_per_room& second);
bool operator!=(const values_per_room& first, const values_per_room& second);

std::ostream& operator<<(std::ostream& out, const values_per_room& to_print);
}
}
#endif  // WOLF_CONTROLLER_VALUES_PER_ROOM_HPP
