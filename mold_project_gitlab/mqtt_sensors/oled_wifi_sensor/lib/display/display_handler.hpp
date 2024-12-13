#ifndef DISPLAY_HANDLER_HPP
#define DISPLAY_HANDLER_HPP

#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <array>
#include <string>

class display_handler {
 public:
  display_handler(const uint8_t screen_width, const uint8_t screen_height,
                  const int8_t reset_pin);
  ~display_handler();

  bool initialize_display(const uint8_t screen_i2c_address);
  void empty_display();
  void change_rotation_time(const unsigned int seconds);
  void start_rotation();
  using sensor_values = std::array<double, 3>;
  void set_sensor_values(const sensor_values &values);
  using units = std::array<std::string, 3>;
  void set_units(const units &units_);
  void sensor_values_display_rotation();
  void set_display_off_time(const unsigned int minutes);

 private:
  void set_temperature_unit(std::string &temp_unit);
  void print_value(const double value, const std::string &unit);
  void display_sensor_value();
  void check_if_off();

 private:
  Adafruit_SSD1306 display;
  sensor_values current_values;
  units sensor_units;
  uint8_t display_index;
  unsigned long check_time_rotate;  // in ms
  unsigned int rotation_time;       // in s
  bool rotate;
  unsigned long check_time_display;  // in ms
  unsigned int display_off_time;     // in min

  bool first_rotation;
};

#endif