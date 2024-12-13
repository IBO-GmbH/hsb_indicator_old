#include "sprout_type_helper.hpp"
#include <boost/stacktrace.hpp>
#include "log/logger.hpp"

Clima::Enums::_bond_enumerators::MoldSproutType::MoldSproutType
mold::network::sprout_type_helper::cast_to_network(
    const mold::sprout_type to_cast) {
  switch (to_cast) {
    case mold::sprout_type::one_day:
      return Clima::Enums::MoldSproutType::OneDay;
    case mold::sprout_type::two_days:
      return Clima::Enums::MoldSproutType::TwoDay;
    case mold::sprout_type::four_days:
      return Clima::Enums::MoldSproutType::FourDay;
    case mold::sprout_type::eight_days:
      return Clima::Enums::MoldSproutType::EightDay;
    case mold::sprout_type::sixteen_days:
      return Clima::Enums::MoldSproutType::SixteenDay;
    case mold::sprout_type::infinite_days:
      return Clima::Enums::MoldSproutType::InfinityDay;
  }
  logging::logger logger{"network::sensor_value_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_network: could not cast " << std::dec
      << static_cast<int>(to_cast)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return Clima::Enums::MoldSproutType::Unknown;
}

mold::sprout_type mold::network::sprout_type_helper::cast_to_wolf(
    const Clima::Enums::_bond_enumerators::MoldSproutType::MoldSproutType
        to_cast) {
  switch (to_cast) {
    case Clima::Enums::MoldSproutType::OneDay:
      return mold::sprout_type::one_day;
    case Clima::Enums::MoldSproutType::TwoDay:
      return mold::sprout_type::two_days;
    case Clima::Enums::MoldSproutType::FourDay:
      return mold::sprout_type::four_days;
    case Clima::Enums::MoldSproutType::EightDay:
      return mold::sprout_type::eight_days;
    case Clima::Enums::MoldSproutType::SixteenDay:
      return mold::sprout_type::sixteen_days;
    case Clima::Enums::MoldSproutType::InfinityDay:
      return mold::sprout_type::infinite_days;
    case Clima::Enums::MoldSproutType::Unknown:
      break;
  }
  logging::logger logger{"network::sensor_value_type_helper"};
  LOGGING_LOG(logger, logging::severity::error)
      << "cast_to_wolf: could not cast " << std::dec
      << static_cast<int>(to_cast)
      << ", stacktrace:" << boost::stacktrace::stacktrace();
  assert(false);
  return mold::sprout_type::infinite_days;
}
