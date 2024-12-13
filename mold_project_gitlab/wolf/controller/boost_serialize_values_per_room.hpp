#ifndef BOOST_SERIALIZE_VALUES_PER_ROOM_HPP
#define BOOST_SERIALIZE_VALUES_PER_ROOM_HPP

#include "boost_serialize_limits.hpp"
#include "values_per_room.hpp"

#include <boost/serialization/array.hpp>
#include <boost/serialization/optional.hpp>

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& archive, wolf::controller::values_per_room& values,
               const unsigned int /*version*/) {
  archive& values.room_id;
  archive& values.limits_;
  archive& values.actors;
  archive& values.active_loops;
}

}  // namespace serialization
}  // namespace boost

#endif  // BOOST_SERIALIZE_VALUES_PER_ROOM_HPP
