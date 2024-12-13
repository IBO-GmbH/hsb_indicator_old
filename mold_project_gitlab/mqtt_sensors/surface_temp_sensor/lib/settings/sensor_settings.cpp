#include "sensor_settings.hpp"

bool operator!=(const sensor_settings& first, const sensor_settings& second) {
  return !(first == second);
}

bool operator==(const sensor_settings& first, const sensor_settings& second) {
  return first.display_turn_off_time == second.display_turn_off_time &&
         first.sensor_read_interval == second.sensor_read_interval &&
         first.temperature_unit == second.temperature_unit &&
         first.sensor_offset == second.sensor_offset;
}

String sensor_settings::to_string() {
  String settings_string;
  settings_string +=
      (String("display_turn_off_time: ") + String(display_turn_off_time));
  settings_string +=
      ("; sensor_read_interval: " + String(sensor_read_interval));
  settings_string +=
      ("; temperature_unit: " + String(temperature_unit.c_str()));
  settings_string +=
      ("; sensor_offset: " + String(sensor_offset));
  return settings_string;
}
