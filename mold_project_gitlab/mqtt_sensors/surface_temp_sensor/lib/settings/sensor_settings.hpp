#ifndef SENSOR_SETTINGS_HPP
#define SENSOR_SETTINGS_HPP

#pragma once

#include <array>
#include <string>
#include <WString.h>

struct sensor_settings {
  unsigned int display_turn_off_time = 10;     // s
  unsigned int sensor_read_interval = 5;      // min
  std::string temperature_unit = "C";
  double sensor_offset = 0.0;

  String to_string();
};

bool operator==(const sensor_settings &first, const sensor_settings &second);
bool operator!=(const sensor_settings &first, const sensor_settings &second);

#endif