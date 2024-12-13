#include "heating_valve_settings.hpp"
#include "wolf/math_compare_decimals.hpp"

bool mold::operator==(const mold::heating_valve_settings &first,
                      const mold::heating_valve_settings &second) {
  return first.room_id == second.room_id &&
         first.manual_mode == second.manual_mode &&
         first.summer_mode == second.summer_mode &&
         wolf::math::compare_floats(first.desired_temperature,
                                    second.desired_temperature) &&
         first.set_valve_position == second.set_valve_position;
}

bool mold::operator!=(const mold::heating_valve_settings &first,
                      const mold::heating_valve_settings &second) {
  return !(first == second);
}

std::ostream &mold::operator<<(std::ostream &out,
                               const mold::heating_valve_settings to_print) {
  out << "{room_id:" << to_print.room_id
      << ", manual_mode:" << to_print.manual_mode
      << ", summer_mode:" << to_print.summer_mode
      << ", desired_temperature:" << to_print.desired_temperature
      << ", set_valve_position:" << to_print.set_valve_position << "}";
  return out;
}
