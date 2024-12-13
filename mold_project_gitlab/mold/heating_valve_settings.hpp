#ifndef MOLD_HEATING_VALVE_SETTINGS_HPP
#define MOLD_HEATING_VALVE_SETTINGS_HPP

#include <ostream>
#include "wolf/types.hpp"

namespace mold {

struct heating_valve_settings {
  wolf::types::uuid_array room_id;
  bool manual_mode;
  bool summer_mode;
  float desired_temperature;
  int set_valve_position;
};

bool operator==(const heating_valve_settings& first,
                const heating_valve_settings& second);
bool operator!=(const heating_valve_settings& first,
                const heating_valve_settings& second);

std::ostream& operator<<(std::ostream& out,
                         const heating_valve_settings to_print);

}  // namespace mold
#endif  // MOLD_HEATING_VALVE_SETTINGS_HPP
