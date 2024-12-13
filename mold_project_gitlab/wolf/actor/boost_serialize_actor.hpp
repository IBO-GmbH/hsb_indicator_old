#ifndef BOOST_SERIALIZE_ACTOR_HPP
#define BOOST_SERIALIZE_ACTOR_HPP

#include "actor.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& archive, wolf::actor::actor& values,
               const unsigned int /*version*/) {
  archive& values.id;
  archive& values.state;
}

}  // namespace serialization
}  // namespace boost

#endif  // BOOST_SERIALIZE_ACTOR_HPP
