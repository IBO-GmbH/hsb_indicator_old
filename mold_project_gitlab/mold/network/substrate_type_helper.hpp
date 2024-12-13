#ifndef MOLD_NETWORK_SUBSTRATE_TYPE_HELPER_HPP
#define MOLD_NETWORK_SUBSTRATE_TYPE_HELPER_HPP

#include "mold_protocol_types.h"
#include "mold/substrate_type.hpp"

namespace mold::network {

class substrate_type_helper {
 public:
  substrate_type_helper() = default;

  ::Clima::Enums::SubstrateType cast_to_network(
      const substrate_type& type) const;
  substrate_type cast_from_network(
      const ::Clima::Enums::SubstrateType& type) const;
};
}  // namespace mold::network

#endif  // MOLD_NETWORK_SUBSTRATE_TYPE_HELPER_HPP
