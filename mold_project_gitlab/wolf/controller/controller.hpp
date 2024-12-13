#ifndef WOLF_CONTROLLER_CONTROLLER_HPP
#define WOLF_CONTROLLER_CONTROLLER_HPP

#include "on_off_control.hpp"
#include "wolf/actor/actor.hpp"

#include <array>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

namespace wolf {

namespace controller {

struct controller {
  static constexpr int actors_per_control_loop = 4;
  static constexpr int control_loops_per_room = 4;
  static constexpr int actors_per_room =
      actors_per_control_loop * control_loops_per_room;
  static constexpr int different_types = 5;

  using optional_actor = boost::optional<actor::actor>;
  using control_array = std::array<on_off_control, different_types>;
  using actor_array = std::array<optional_actor, actors_per_room>;

  control_array controller_;
  actor_array actors;
};

std::ostream& operator<<(std::ostream& out, const controller& to_print);
}
}

#endif  // WOLF_CONTROLLER_CONTROLLER_HPP
