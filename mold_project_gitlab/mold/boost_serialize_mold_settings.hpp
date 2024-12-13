#ifndef MOLD_BOOST_SERIALIZE_MOLD_SETTINGS_HPP
#define MOLD_BOOST_SERIALIZE_MOLD_SETTINGS_HPP

#include <boost/serialization/version.hpp>
#include "mold/mold_settings.hpp"

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive& ar, mold::mold_settings& config,
               const unsigned int version) {
  ar& config.building_type_;
  ar& config.frsi;
  if (version > 0) ar& config.substrate_type_;
}

}  // namespace serialization
}  // namespace boost

BOOST_CLASS_VERSION(mold::mold_settings, 1);

#endif  // MOLD_BOOST_SERIALIZE_MOLD_SETTINGS_HPP
