#ifndef WOLF_ACTOR_ACTOR_SETTER_HPP
#define WOLF_ACTOR_ACTOR_SETTER_HPP

#include "indexes.hpp"
#include "log/logger.hpp"
#include "wolf/controller/controller_per_room.hpp"
#include "wolf/controller/svas2_dehumidifier.hpp"
#include "wolf/controller/svas3_heating.hpp"

#include <sstream>

namespace wolf {

namespace actor {

class actor_setter {
 public:
  using controller_per_room_list = std::vector<controller::controller_per_room>;

  actor_setter();
  void set_actor(const controller_per_room_list::iterator& found,
                 const unsigned int index, const bool state,
                 std::stringstream& string);
  void set_actors(const controller_per_room_list::iterator& found,
                  const indexes& indexes, const bool state,
                  std::stringstream& string);
  bool set_actors_svas2(
      const controller_per_room_list::iterator& found,
      const wolf::controller::svas2_dehumidifier::outputs& outputs,
      std::stringstream& string);
  bool set_actors_svas3(const controller_per_room_list::iterator& found,
                        const wolf::controller::svas3_heating::outputs& outputs,
                        std::stringstream& string);

 private:
  logging::logger m_logger;
};
}  // namespace actor
}  // namespace wolf

#endif  // WOLF_ACTOR_ACTOR_SETTER_HPP
