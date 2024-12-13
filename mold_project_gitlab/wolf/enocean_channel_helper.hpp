#ifndef WOLF_ENOCEAN_CHANNEL_HELPER_HPP
#define WOLF_ENOCEAN_CHANNEL_HELPER_HPP

#include <Includes/eoChannelEnums.h>
#include <optional>
#include "enocean_subchannel_type.hpp"
#include "types.hpp"
#include "wolf/sensor_value_type.hpp"

namespace wolf {

class enocean_channel_helper {
 public:
  enocean_channel_helper() = default;

  static sensor_value_type cast_to_wolf(
      const CHANNEL_TYPE type,
      const std::optional<enocean_subchannel_type>& subchannel,
      const types::eep& eep);
  static std::optional<types_with_subchannel> get_type_with_subchannel(
      const CHANNEL_TYPE type);
  static bool eep_is_heating_valve(const types::eep& eep);

 private:
  static bool eep_is_presence(const types::eep& eep);
};
}  // namespace wolf
#endif
