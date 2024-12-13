#ifndef MOLD_ROOM_CREATOR_HPP
#define MOLD_ROOM_CREATOR_HPP

#include "mold/configuration_handler.hpp"
#include "wolf/room_handler.hpp"

namespace mold {

class room_creator {
 public:
  room_creator(wolf::room_handler& rooms,
               mold::configuration_handler& configurations);

  struct result {
    wolf::room room;
    configuration configuration_;
  };
  // throws!
  //  result create(const std::string& name,
  //                const std::vector<wolf::sensor_profile>& profiles);
  // throws!
  result create(const wolf::types::uuid_array &room_id, const std::string& name,
                const std::vector<wolf::sensor_id>& sensors,
                const wolf::types::uuid_array& initial_id,
                const bool is_initial);

 private:
  logging::logger m_logger;
  wolf::room_handler& m_rooms;
  mold::configuration_handler& m_configurations;
};
}  // namespace mold
#endif
