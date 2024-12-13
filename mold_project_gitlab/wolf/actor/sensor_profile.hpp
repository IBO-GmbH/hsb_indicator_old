#ifndef WOLF_ACTOR_SENSOR_PROFILE_HPP
#define WOLF_ACTOR_SENSOR_PROFILE_HPP

#include "actor.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/room_handler.hpp"

namespace wolf {

namespace actor {

class sensor_profile {
 public:
  static sensor_id create_sensor_id(const types::id_esp3& actor_esp3_id);
  static void add_actor_switch_to_room(const actor& actor,
                                       const types::id_esp3& base_id,
                                       room& room_, room_handler& room_handler,
                                       profile_handler& profile_handler);

 private:
  static wolf::sensor_profile create_sensor_profile(
      const actor& actor_, const types::id_esp3& base_id);
};
}  // namespace actor
}  // namespace wolf
#endif  // WOLF_ACTOR_SENSOR_PROFILE_HPP
