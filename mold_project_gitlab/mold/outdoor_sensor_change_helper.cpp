#include "outdoor_sensor_change_helper.hpp"
#include "sensor_to_room_and_configuration_helper.hpp"

using namespace mold;
using namespace logging;

outdoor_sensor_change_helper::outdoor_sensor_change_helper(
    configuration_handler &config_handler, wolf::room_handler &room_handler,
    room_and_configuration_saver &saver)
    : m_logger("outdoor_sensor_change_helper"),
      m_config_handler(config_handler),
      m_room_handler(room_handler),
      m_saver(saver) {}

void outdoor_sensor_change_helper::set(const wolf::outdoor_sensor &to_set) {
  m_config_handler.set_outdoor_sensor(to_set);
  const bool outdoor_temp_set = to_set.temperature.is_set();
  if (!outdoor_temp_set) return;
  sensor_to_room_and_configuration_helper sensor_room_config_helper;
  for (const auto &room : m_room_handler.get_all()) {
    auto room_copy(room);
    auto configuration_copy(m_config_handler.get_by_room_id(room.id));
    if (!configuration_copy.has_value()) continue;
    const auto sensors = room.sensors;
    sensor_room_config_helper.handle(
        sensors, room_copy, configuration_copy.get(), outdoor_temp_set);
    m_saver.save_and_update(room_copy, configuration_copy.get());
    signal_valid_outdoor(configuration_copy->id);
  }
}
