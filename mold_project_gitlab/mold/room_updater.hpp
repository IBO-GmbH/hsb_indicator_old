#ifndef MOLD_ROOM_UPDATER_HPP
#define MOLD_ROOM_UPDATER_HPP

#include "mold/configuration_saver.hpp"
#include "mold/room_saver.hpp"
#include "mold/sensor_to_room_and_configuration_helper.hpp"

namespace mold {

class room_updater {
 public:
  room_updater(mold::room_saver& room_saver,
               const mold::configuration_handler& config_handler,
               mold::configuration_saver& config_saver);
  bool update_room_and_config(wolf::room& room);

 private:
  mold::room_saver& m_room_saver;
  const mold::configuration_handler& m_config_handler;
  mold::configuration_saver& m_config_saver;
  mold::sensor_to_room_and_configuration_helper m_room_config_helper;
};
}  // namespace mold

#endif
