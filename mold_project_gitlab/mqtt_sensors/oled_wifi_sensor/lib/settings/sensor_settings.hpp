#ifndef SENSOR_SETTINGS_HPP
#define SENSOR_SETTINGS_HPP

#pragma once

#include <array>
#include <string>
#include <WString.h>

struct sensor_settings {
  bool proximity_enabled = false;
  unsigned int display_turn_off_time = 10;     // min
  unsigned int display_rotation_interval = 5;  // s
  unsigned int sensor_read_interval = 30;      // s
  std::string temperature_unit = "C";
  std::string pressure_unit = "hpa";
  std::array<double, 3> sensor_offsets = {0.0, 0.0, 0.0};

  String to_string();
};

bool operator==(const sensor_settings &first, const sensor_settings &second);
bool operator!=(const sensor_settings &first, const sensor_settings &second);

#endif