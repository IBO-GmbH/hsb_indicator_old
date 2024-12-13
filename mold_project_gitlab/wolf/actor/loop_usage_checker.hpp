#ifndef WOLF_ACTOR_LOOP_USAGE_CHECKER_HPP
#define WOLF_ACTOR_LOOP_USAGE_CHECKER_HPP

#include "actor.hpp"
#include "wolf/controller/controller_per_room.hpp"

#include <boost/optional.hpp>

namespace wolf {

namespace actor {

class loop_usage_checker {
 public:
  using optional_actor = boost::optional<actor>;
  using actor_list_pointer = std::vector<optional_actor*>;
  using actor_array = std::array<optional_actor, 16>;
  using controller_list = std::vector<controller::controller_per_room>;

  inline static bool used_in_other_loops(const actor_array& all_actors,
                                         const actor& actor_to_check) {
    return used_in_other_loops(all_actors, actor_to_check, 1);
  }
  static bool used_in_other_loops(const actor_array& all_actors,
                                  const actor& actor_to_check, const int check);
  static actor_list_pointer get_actors_of_loop(actor_array& all_actors,
                                               const unsigned int start,
                                               const unsigned int end);
  static bool used_in_other_rooms(const controller_list& controller_list_,
                                  const wolf::types::uuid_array& room_id,
                                  const actor& actor_to_check);
};
}  // namespace actor
}  // namespace wolf
#endif  // WOLF_ACTOR_LOOP_USAGE_CHECKER_HPP
