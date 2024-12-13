#ifndef MOLD_SYNC_BOX_SETTINGS_ENTITY_HPP
#define MOLD_SYNC_BOX_SETTINGS_ENTITY_HPP

#include "mold/building_type.hpp"
#include "mold/substrate_type.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/sensor_profile.hpp"

namespace mold::sync::box_settings {

struct entity {
  wolf::outdoor_sensor outdoor_sensor;
  building_type building_type_;
  float threshold_factor;
  substrate_type substrate_type_;
};

std::ostream& operator<<(std::ostream& out, const entity& to_print);

}  // namespace mold::sync::box_settings

#endif
