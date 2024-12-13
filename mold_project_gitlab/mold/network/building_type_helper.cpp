#include "building_type_helper.hpp"
#include <boost/stacktrace.hpp>
#include "log/logger.hpp"

using namespace mold::network;

Clima::Enums::BuildingType building_type_helper::cast_to_network(
    const building_type &type) const {
  switch (type) {
    case mold::building_type::old:
      return Clima::Enums::BuildingType::Old;
    case mold::building_type::new_:
      return Clima::Enums::BuildingType::New;
    case mold::building_type::none:
      return Clima::Enums::BuildingType::NotSet;
  }
  logging::logger logger{"network::building_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_network: could not cast " << std::dec
      << static_cast<int>(type)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return Clima::Enums::BuildingType::NotSet;
}

mold::building_type building_type_helper::cast_from_network(
    const Clima::Enums::_bond_enumerators::BuildingType::BuildingType &type)
    const {
  switch (type) {
    case Clima::Enums::BuildingType::Old:
      return mold::building_type::old;
    case Clima::Enums::BuildingType::New:
      return mold::building_type::new_;
    case Clima::Enums::BuildingType::NotSet:
      return mold::building_type::none;
  }
  logging::logger logger{"network::building_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_wolf: could not cast " << std::dec << static_cast<int>(type)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return mold::building_type::none;
}
