#include "substrate_type_helper.hpp"
#include <boost/stacktrace.hpp>
#include "log/logger.hpp"

using namespace mold::network;

Clima::Enums::_bond_enumerators::SubstrateType::SubstrateType
substrate_type_helper::cast_to_network(const mold::substrate_type &type) const {
  switch (type) {
    case mold::substrate_type::type_one:
      return Clima::Enums::SubstrateType::TypeOne;
    case mold::substrate_type::type_two:
      return Clima::Enums::SubstrateType::TypeTwo;
    case mold::substrate_type::none:
      return Clima::Enums::SubstrateType::NotSet;
  }
  logging::logger logger{"network::substrate_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_network: could not cast " << std::dec
      << static_cast<int>(type)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return Clima::Enums::SubstrateType::NotSet;
}

mold::substrate_type substrate_type_helper::cast_from_network(
    const Clima::Enums::_bond_enumerators::SubstrateType::SubstrateType &type)
    const {
  switch (type) {
    case Clima::Enums::SubstrateType::TypeOne:
      return mold::substrate_type::type_one;
    case Clima::Enums::SubstrateType::TypeTwo:
      return mold::substrate_type::type_two;
    case Clima::Enums::SubstrateType::NotSet:
      return mold::substrate_type::none;
  }
  logging::logger logger{"network::substrate_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_network: could not cast " << std::dec
      << static_cast<int>(type)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return mold::substrate_type::none;
}
