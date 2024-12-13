#ifndef MOLD_ROOM_AND_CONFIGURATION_SAVER_HPP
#define MOLD_ROOM_AND_CONFIGURATION_SAVER_HPP

#include "mold/configuration_saver.hpp"
#include "mold/room_saver.hpp"

namespace mold {

class room_and_configuration_saver {
 public:
  room_and_configuration_saver(room_saver& room_saver_,
                               configuration_saver& configuration_saver_);

  std::pair<wolf::room, configuration> save_and_update(
      wolf::room room, configuration configuration_);

 private:
  room_saver& m_room_saver;
  configuration_saver& m_configuration_saver;
};

}  // namespace mold

#endif  // MOLD_ROOM_AND_CONFIGURATION_SAVER_HPP
