#ifndef MOLD_SENSOR_ID_MQTT_HPP
#define MOLD_SENSOR_ID_MQTT_HPP

#include "wolf/sensor_id.hpp"

namespace mold {

class sensor_id_mqtt {
 public:
  static wolf::sensor_id create(const std::string& client_id,
                                const wolf::sensor_value_type& value_type);
};
}  // namespace mold

#endif  // MOLD_SENSOR_ID_MQTT_HPP
