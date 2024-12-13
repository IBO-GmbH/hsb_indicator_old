#ifndef MOLD_SYNC_SENSOR_ID_FRSI_HPP
#define MOLD_SYNC_SENSOR_ID_FRSI_HPP

#include "wolf/sensor_id.hpp"

namespace mold::sync {

class sensor_id_frsi {
 public:
  static wolf::sensor_id create(const wolf::types::uuid_array& config_id);
};
}  // namespace mold::sync

#endif  // MOLD_SYNC_SENSOR_ID_FRSI_HPP
