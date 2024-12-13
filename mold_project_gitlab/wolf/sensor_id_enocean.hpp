#ifndef WOLF_SENSOR_ID_ENOCEAN_HPP
#define WOLF_SENSOR_ID_ENOCEAN_HPP

#include <Includes/eoChannelEnums.h>
#include <optional>
#include "enocean_subchannel_type.hpp"
#include "sensor_id.hpp"

namespace wolf {

class sensor_id_enocean {
 public:
  static sensor_id create(
      const types::id_esp3& esp3_id, const CHANNEL_TYPE& channel,
      const std::optional<enocean_subchannel_type>& subchannel,
      const types::eep& eep);
  static types::id_esp3 convert_to_esp3_id(const sensor_id& to_convert);
  static std::string esp3_id_string(const wolf::types::id_esp3& id);
  static std::string eep_string(const wolf::types::eep& eep);
};
}  // namespace wolf

#endif  // WOLF_SENSOR_ID_ENOCEAN_HPP
