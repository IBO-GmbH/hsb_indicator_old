#include "sensor_settings.hpp"

bool operator!=(const sensor_settings& first, const sensor_settings& second) {
  return !(first == second);
}

bool operator==(const sensor_settings& first, const sensor_settings& second) {
  return first.proximity_enabled == second.proximity_enabled &&
         first.display_turn_off_time == second.display_turn_off_time &&
         first.display_rotation_interval == second.display_rotation_interval &&
         first.sensor_read_interval == second.sensor_read_interval &&
         first.temperature_unit == second.temperature_unit &&
         first.pressure_unit == second.pressure_unit &&
         first.sensor_offsets == second.sensor_offsets;
}

String sensor_settings::to_string() {
  String settings_string;
  settings_string +=
      (String("proximity_enabled: ") + (proximity_enabled ? "true" : "false"));
  settings_string +=
      ("; display_turn_off_time: " + String(display_turn_off_time));
  settings_string +=
      ("; display_rotation_interval: " + String(display_rotation_interval));
  settings_string +=
      ("; sensor_read_interval: " + String(sensor_read_interval));
  settings_string +=
      ("; temperature_unit: " + String(temperature_unit.c_str()));
  settings_string += ("; pressure_unit: " + String(pressure_unit.c_str()));
  settings_string +=
      ("; sensor_offsets: " + String(sensor_offsets[0]) + ", " +
       String(sensor_offsets[1]) + ", " + String(sensor_offsets[2]));
  return settings_string;
}
