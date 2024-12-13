#ifndef MOLD_SENSOR_ID_FROM_UUID_AND_TYPES_HPP
#define MOLD_SENSOR_ID_FROM_UUID_AND_TYPES_HPP

#include "wolf/sensor_id.hpp"

namespace mold {

class sensor_id_from_uuid_and_types {
 public:
  static wolf::sensor_id create(const wolf::types::uuid_array& uuid,
                                const wolf::sensor_type type,
                                const wolf::sensor_value_type value_type);
};

}  // namespace mold
#endif
