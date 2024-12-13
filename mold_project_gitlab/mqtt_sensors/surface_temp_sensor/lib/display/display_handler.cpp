#include "display_handler.hpp"

#include <M5StickCPlus.h>

display_handler::display_handler()
    : check_time_display(millis()), is_init(false) {}

display_handler::~display_handler() { empty_display(); }

void display_handler::init_display() {
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(WHITE);
  M5.Axp.ScreenBreath(8);
  empty_display();
  is_init = true;
}

void display_handler::empty_display() { M5.Lcd.fillScreen(BLACK); }

void display_handler::display_sensor_value(const double value,
                                           const std::string &unit) {
  check_time_display = millis();
  const double converted_value = convert_value(value, unit);
  print_sensor_value(converted_value, unit);
}

void display_handler::display_string(const std::string &to_display) {
  if (!is_init) return;
  empty_display();
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 50);
  M5.Lcd.printf("%s", to_display.c_str());
}

void display_handler::print_sensor_value(const double value,
                                         const std::string &unit) {
  empty_display();
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(30, 50);
  M5.Lcd.printf("%.1lf", value);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(M5.Lcd.getCursorX(), 45);
  M5.Lcd.printf("o");
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(M5.Lcd.getCursorX(), 50);
  M5.Lcd.printf("%s", unit.c_str());
}

double display_handler::convert_value(const double value,
                                      const std::string &unit) {
  // if unit is °F convert value
  if (unit.find("F") != std::string::npos) return value * 1.8 + 32;
  return value;
}

bool display_handler::check_if_off(const unsigned int display_off_time,
                                   const bool is_config) {
  auto current_time = millis();
  if (((current_time - check_time_display) < (display_off_time * 1000)) ||
      is_config)
    return false;
  check_time_display = current_time;
  empty_display();
  return true;
}

void display_handler::display_ip_address(const std::string &address) {
  check_time_display = millis();
  display_string(address);
}
