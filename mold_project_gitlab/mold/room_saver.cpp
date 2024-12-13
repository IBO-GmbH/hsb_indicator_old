#include "room_saver.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_generator.hpp"

using namespace mold;

room_saver::room_saver(wolf::room_handler &room_handler)
    : m_logger{"room_saver"}, m_room_handler(room_handler) {}

wolf::room room_saver::save_and_update(wolf::room room) {
  wolf::thread_checker::throw_if_not_service("room_saver::save_and_update");

  const bool isAdd = (room.id == wolf::types::uuid_array{});
  if (isAdd) {
    room.id = wolf::uuid_generator::generate_as_array();
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "saving new, room:" << room;
    m_room_handler.add(room);
    return room;
  }
  LOGGING_LOG(m_logger, logging::severity::normal) << "updating, room:" << room;
  m_room_handler.update(room);
  return room;
}
