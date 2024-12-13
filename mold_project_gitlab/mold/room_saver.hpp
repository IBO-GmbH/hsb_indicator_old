#ifndef MOLD_ROOM_SAVER_HPP
#define MOLD_ROOM_SAVER_HPP

#include "wolf/room_handler.hpp"

namespace mold {

class room_saver {
 public:
  room_saver(wolf::room_handler& room_handler);

  wolf::room save_and_update(wolf::room room);

 private:
  logging::logger m_logger;
  wolf::room_handler& m_room_handler;
};

}  // namespace mold

#endif  // MOLD_ROOM_SAVER_HPP
