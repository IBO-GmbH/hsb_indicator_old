#ifndef MOLD_BOOST_SERIALIZE_SPROUT_VALUE_HPP
#define MOLD_BOOST_SERIALIZE_SPROUT_VALUE_HPP

#include "wolf/boost_serialize_chrono_system_clock_timepoint.hpp"
#include "mold/sprout_value.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, mold::sprout_value& value,
               const unsigned int /*version*/) {
  ar& value.sprout_type_;
  ar& value.timestamp_start;
  ar& value.timestamp_change;
}

}  // namespace serialization
}  // namespace boost

#endif  // MOLD_BOOST_SERIALIZE_SPROUT_VALUE_HPP
