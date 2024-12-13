#ifndef SENSOR_VALUE_READER_HPP
#define SENSOR_VALUE_READER_HPP

#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <array>

class sensor_value_reader {
 public:
  sensor_value_reader();
  ~sensor_value_reader() = default;

  void sensor_value_init();
  bool handle_read(const int address);
  using sensor_values = std::array<double, 3>;
  sensor_values get_sensor_values();
  void change_read_interval(const unsigned int interval);
  void change_sensor_offsets(const sensor_values& new_offsets);

 private:
  unsigned int read_interval;  // in s
  std::array<unsigned char, 8> i2c_response;
  sensor_values sensor_values_;
  unsigned long check_time;  // in ms
  sensor_values offsets;

  bool first_read;
};

#endif