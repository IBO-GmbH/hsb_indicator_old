#ifndef MOLD_BOOST_SERIALIZE_CONFIGURATION_STATE_HPP
#define MOLD_BOOST_SERIALIZE_CONFIGURATION_STATE_HPP

#include "wolf/boost_serialize_chrono_system_clock_timepoint.hpp"
#include "mold/configuration_state.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& archive, mold::configuration_state& to_save_load,
               const unsigned int) {
  archive& to_save_load.state;
  archive& to_save_load.time_since_first_green;
  archive& to_save_load.time_since_green;
  archive& to_save_load.time_since_not_green;
}

}  // namespace serialization
}  // namespace boost

#endif  // MOLD_BOOST_SERIALIZE_CONFIGURATION_STATE_HPP
