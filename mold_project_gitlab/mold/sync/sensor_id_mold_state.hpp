#ifndef MOLD_SYNC_SENSOR_ID_MOLD_STATE_HPP
#define MOLD_SYNC_SENSOR_ID_MOLD_STATE_HPP

#include "wolf/sensor_id.hpp"

namespace mold::sync {
class sensor_id_mold_state {
 public:
  static wolf::sensor_id create(const wolf::types::uuid_array& config_id);
};
}  // namespace mold::sync

#endif
