#ifndef MOLD_BOOST_SERIALIZE_CONFIGURATION_HPP
#define MOLD_BOOST_SERIALIZE_CONFIGURATION_HPP

#include "mold/boost_serialize_mold_settings.hpp"
#include "mold/configuration.hpp"
#include "wolf/boost_serialize_sensor_id.hpp"

#include <boost/serialization/optional.hpp>

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, mold::configuration& config,
               const unsigned int /*version*/) {
  ar& config.id;
  ar& config.room;
  ar& config.temperature;
  ar& config.humidity;
  ar& config.mold_settings_;
}

}  // namespace serialization
}  // namespace boost

#endif  // MOLD_BOOST_SERIALIZE_CONFIGURATION_HPP
