#ifndef WOLF_BOOST_SERIALIZE_SENSOR_PROFILE_HPP
#define WOLF_BOOST_SERIALIZE_SENSOR_PROFILE_HPP

#include <boost/serialization/version.hpp>
#include "wolf/boost_serialize_sensor_metadata.hpp"
#include "wolf/sensor_profile.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, wolf::sensor_profile& profile,
               const unsigned int version) {
  ar& profile.id;
  ar& profile.type;
  if (version > 0) ar& profile.metadata;
}
}  // namespace serialization
}  // namespace boost

BOOST_CLASS_VERSION(wolf::sensor_profile, 1);

#endif  // WOLF_BOOST_SERIALIZE_SENSOR_PROFILE_HPP
