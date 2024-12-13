#ifndef DISPLAY_HANDLER_HPP
#define DISPLAY_HANDLER_HPP

#pragma once

#include <array>
#include <string>

class display_handler {
 public:
  display_handler();
  ~display_handler();

  void init_display();
  void empty_display();
  void display_ip_address(const std::string &address);
  bool check_if_off(const unsigned int display_off_time, const bool is_config);
  void display_sensor_value(const double value, const std::string &unit);
  void display_string(const std::string &to_display);

 private:
  void print_sensor_value(const double value, const std::string &unit);
  double convert_value(const double value, const std::string &unit);

 private:
  unsigned long check_time_display;  // in ms
  bool is_init;
};

#endif