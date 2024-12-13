#ifndef MOLD_OUTDOOR_SENSOR_CHANGE_HELPER_HPP
#define MOLD_OUTDOOR_SENSOR_CHANGE_HELPER_HPP

#include "log/logger.hpp"
#include "mold/configuration_handler.hpp"
#include "mold/room_and_configuration_saver.hpp"
#include "wolf/room_handler.hpp"

namespace mold {

class outdoor_sensor_change_helper {
 public:
  outdoor_sensor_change_helper(configuration_handler& config_handler,
                               wolf::room_handler& room_handler,
                               room_and_configuration_saver& saver);

  void set(const wolf::outdoor_sensor& to_set);
  boost::signals2::signal<void(const wolf::types::uuid_array&)>
      signal_valid_outdoor;

 private:
  logging::logger m_logger;
  configuration_handler& m_config_handler;
  wolf::room_handler& m_room_handler;
  room_and_configuration_saver& m_saver;
};
}  // namespace mold

#endif  // MOLD_OUTDOOR_SENSOR_CHANGE_HELPER_HPP
