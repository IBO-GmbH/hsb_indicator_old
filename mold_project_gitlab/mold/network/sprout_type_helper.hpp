#ifndef MOLD_NETWORK_SPROUT_TYPE_HELPER_HPP
#define MOLD_NETWORK_SPROUT_TYPE_HELPER_HPP

#include "mold_protocol_types.h"
#include "mold/sprout_type.hpp"

namespace mold::network {

class sprout_type_helper {
 public:
  sprout_type_helper() = default;

  Clima::Enums::MoldSproutType cast_to_network(const sprout_type to_cast);
  sprout_type cast_to_wolf(const Clima::Enums::MoldSproutType to_cast);
};
}  // namespace mold::network
#endif  // MOLD_NETWORK_SPROUT_TYPE_HELPER_HPP
