#ifndef MOLD_SENSOR_ID_EVALUATION_HUMIDITY_HPP
#define MOLD_SENSOR_ID_EVALUATION_HUMIDITY_HPP

#include "wolf/sensor_id.hpp"

namespace mold {

// TODO move to evaluation_humidity::
class sensor_id_evaluation_humidity {
 public:
  static wolf::sensor_id create(const wolf::types::uuid_array& config_id);
};
}  // namespace mold

#endif
