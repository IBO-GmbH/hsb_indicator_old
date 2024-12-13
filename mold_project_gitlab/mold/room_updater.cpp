#include "room_updater.hpp"

using namespace mold;

room_updater::room_updater(room_saver &room_saver,
                           const configuration_handler &config_handler,
                           configuration_saver &config_saver)
    : m_room_saver(room_saver),
      m_config_handler(config_handler),
      m_config_saver(config_saver) {}

bool room_updater::update_room_and_config(wolf::room &room) {
  auto config = m_config_handler.get_by_room_id(room.id);
  if (!config.has_value()) {
    m_room_saver.save_and_update(room);
    return false;
  }
  auto sensors = room.sensors;
  m_room_config_helper.handle(
      sensors, room, config.get(),
      m_config_handler.get_outdoor_sensor().temperature.is_set());
  m_room_saver.save_and_update(room);
  m_config_saver.save_and_update(config.get());
  return true;
}
