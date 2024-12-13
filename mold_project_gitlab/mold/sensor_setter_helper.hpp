#ifndef MOLD_SENSOR_SETTER_HELPER_HPP
#define MOLD_SENSOR_SETTER_HELPER_HPP

#include "mold/configuration.hpp"
#include "mold/configuration_handler.hpp"
#include "mold/room_and_configuration_saver.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_profile.hpp"
#include "wolf/sensor_value.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold {

class sensor_setter_helper {
 public:
  sensor_setter_helper(room_and_configuration_saver& room_config_saver,
                       wolf::profile_handler& profile_handler,
                       wolf::sensor_value_handler& value_handler);

  std::pair<wolf::room, configuration> set_to_room_and_configuration(const wolf::room& room, const configuration& configuration,
      const std::vector<wolf::sensor_profile> &new_profiles,
      const std::vector<wolf::sensor_value>& values);

 private:
  logging::logger m_logger;
  room_and_configuration_saver& m_room_and_configuration_saver;
  wolf::profile_handler& m_profile_handler;
  wolf::sensor_value_handler& m_value_handler;
  wolf::room m_room;
  configuration m_configuration;
};

}  // namespace mold

#endif  // SENSOR_SETTER_HELPER_HPP
