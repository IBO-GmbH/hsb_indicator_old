#ifndef MOLD_NETWORK_BUILDING_TYPE_HELPER_HPP
#define MOLD_NETWORK_BUILDING_TYPE_HELPER_HPP

#include "mold_protocol_types.h"
#include "mold/building_type.hpp"

namespace mold::network {

class building_type_helper {
 public:
  building_type_helper() = default;

  ::Clima::Enums::BuildingType cast_to_network(const building_type& type) const;
  building_type cast_from_network(
      const ::Clima::Enums::BuildingType& type) const;
};

}  // namespace mold::network

#endif
