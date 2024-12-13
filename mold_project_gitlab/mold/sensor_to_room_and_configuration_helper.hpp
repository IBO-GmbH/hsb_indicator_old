#ifndef MOLD_SENSOR_TO_ROOM_AND_CONFIGURATION_HELPER_HPP
#define MOLD_SENSOR_TO_ROOM_AND_CONFIGURATION_HELPER_HPP

#include <boost/optional.hpp>
#include "log/logger.hpp"
#include "mold/configuration.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/room.hpp"
#include "wolf/types.hpp"

namespace mold {

class sensor_to_room_and_configuration_helper {
 public:
  sensor_to_room_and_configuration_helper();
  void handle(const std::vector<wolf::sensor_id>& new_sensors_for_room,
              wolf::room& room_, configuration& configuration_,
              const bool outdoor_valid_for_config);

 private:
  void add_sensor_to_room_if_valid(wolf::room& room_,
                                   const wolf::sensor_id& sensor_id);

  logging::logger m_logger;
};
}  // namespace mold

#endif  // MOLD_SENSOR_TO_ROOM_AND_CONFIGURATION_HELPER_HPP
