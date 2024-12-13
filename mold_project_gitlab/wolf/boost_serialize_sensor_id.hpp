#ifndef WOLF_BOOST_SERIALIZE_sensor_id_HPP
#define WOLF_BOOST_SERIALIZE_sensor_id_HPP

#include <boost/serialization/split_free.hpp>
#include "wolf/sensor_id.hpp"

BOOST_SERIALIZATION_SPLIT_FREE(wolf::sensor_id)
namespace boost {
namespace serialization {

template <class Archive>
void save(Archive& ar, const wolf::sensor_id& sensor_id, unsigned int) {
  ar << sensor_id.to_combined_id();
}
template <class Archive>
void load(Archive& ar, wolf::sensor_id& sensor_id, unsigned int) {
  wolf::types::data to_cast;
  ar >> to_cast;
  sensor_id = wolf::sensor_id::from_combined_id(to_cast);
}

}  // namespace serialization
}  // namespace boost

#endif  // WOLF_BOOST_SERIALIZE_sensor_id_HPP
