#include "sensor_setter_helper.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;

sensor_setter_helper::sensor_setter_helper(
    room_and_configuration_saver &room_config_saver,
    wolf::profile_handler &profile_handler, wolf::sensor_value_handler &value_handler)
    : m_logger("sensor_setter_helper"),
      m_room_and_configuration_saver(room_config_saver),
      m_profile_handler(profile_handler),
      m_value_handler(value_handler) {}

std::pair<wolf::room, configuration>
sensor_setter_helper::set_to_room_and_configuration(
    const wolf::room &room, const configuration &configuration_,
    const std::vector<wolf::sensor_profile> &new_profiles,
    const std::vector<wolf::sensor_value> &values) {
  assert(wolf::thread_checker::is_service());
  for (const auto &profile : new_profiles) m_profile_handler.learn(profile);
  const auto result =
      m_room_and_configuration_saver.save_and_update(room, configuration_);
  m_value_handler.handle_list(values);
  return result;
}
