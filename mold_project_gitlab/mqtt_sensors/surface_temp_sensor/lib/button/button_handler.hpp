#ifndef BUTTON_HANDLER_HPP
#define BUTTON_HANDLER_HPP

#include <LittleFS.h>
#include <M5StickCPlus.h>

#include "display_handler.hpp"
#include "wifi_connection.hpp"

#pragma once

class button_handler {
 public:
  enum struct pressed_state { not_pressed, single_press, long_press, multi_press };

  button_handler(Button& button, display_handler& display_,
                 wifi_connection& wifi_, const int multi_press_number_,
                 const int multi_press_delay_, const int long_press_time_);
  ~button_handler();

  pressed_state check_pressed_state(const double value,
                                    const std::string& unit);

 private:
  bool check_single_press();
  bool check_long_press();
  bool check_multi_press();
  pressed_state handle_single_press();
  pressed_state handle_long_press();
  pressed_state handle_multi_press();

 private:
  Button& button_to_check;
  display_handler& display;
  wifi_connection& wifi;
  unsigned int multi_press_number;
  unsigned int multi_press_delay;
  unsigned int long_press_time;
  bool button_pressed;
  unsigned int press_count;
};

#endif