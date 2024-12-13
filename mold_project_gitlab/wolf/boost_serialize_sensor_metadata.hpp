#ifndef WOLF_BOOST_SERIALIZE_SENSOR_METADATA_HPP
#define WOLF_BOOST_SERIALIZE_SENSOR_METADATA_HPP

#include <boost/serialization/map.hpp>
#include "wolf/sensor_metadata.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, wolf::sensor_metadata& metadata,
               const unsigned int /*version*/) {
  ar& metadata.type;
  ar& metadata.min_value;
  ar& metadata.max_value;
  ar& metadata.properties;
}
}  // namespace serialization
}  // namespace boost

#endif  // WOLF_BOOST_SERIALIZE_SENSOR_METADATA_HPP
