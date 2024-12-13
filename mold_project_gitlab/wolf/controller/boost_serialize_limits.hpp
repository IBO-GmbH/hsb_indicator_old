#ifndef BOOST_SERIALIZE_CONTROLLER_HPP
#define BOOST_SERIALIZE_CONTROLLER_HPP

#include "boost_serialize_chrono_system_clock_duration.hpp"
#include "limits.hpp"
#include "wolf/actor/boost_serialize_actor.hpp"

#include <boost/serialization/array.hpp>
#include <boost/serialization/version.hpp>

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& archive, wolf::controller::limits& values,
               const unsigned int version) {
  archive& values.humidity.max;
  archive& values.humidity.drH;
  archive& values.humidity_border;
  archive& values.temperature_.min;
  archive& values.temperature_.reference;
  if (version == 0) archive& values.time;
  archive& values.air_quality_.co2;
  archive& values.air_quality_.sprout_value;
  archive& values.air_quality_.voc;
  archive& values.outdoor_temperature;
  if (version == 2) {
    archive& values.air_quality_.co2_difference;
    archive& values.air_quality_.sprout_value_difference;
    archive& values.air_quality_.voc_difference;
  }
}

}  // namespace serialization
}  // namespace boost

BOOST_CLASS_VERSION(wolf::controller::limits, 2);

#endif  // BOOST_SERIALIZE_CONTROLLER_HPP
