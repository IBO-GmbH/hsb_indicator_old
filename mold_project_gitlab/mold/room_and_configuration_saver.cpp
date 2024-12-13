#include "room_and_configuration_saver.hpp"

using namespace mold;

room_and_configuration_saver::room_and_configuration_saver(
    room_saver &room_saver_, configuration_saver &configuration_saver_)
    : m_room_saver(room_saver_), m_configuration_saver(configuration_saver_) {}

std::pair<wolf::room, configuration>
room_and_configuration_saver::save_and_update(wolf::room room,
                                              configuration configuration_) {
  room = m_room_saver.save_and_update(room);
  configuration_.room = room.id;
  configuration_ = m_configuration_saver.save_and_update(configuration_);
  return std::make_pair(room, configuration_);
}
