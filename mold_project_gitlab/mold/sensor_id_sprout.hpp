#ifndef MOLD_SENSOR_ID_SPROUT_HPP
#define MOLD_SENSOR_ID_SPROUT_HPP

#include "wolf/sensor_id.hpp"

namespace mold {

// TODO move to sprout::
class sensor_id_sprout {
 public:
  static wolf::sensor_id create(const wolf::types::uuid_array& config_id);
};
}  // namespace mold

#endif  // MOLD_SENSOR_ID_SPROUT_HPP
