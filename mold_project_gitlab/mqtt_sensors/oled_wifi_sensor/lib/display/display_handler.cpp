#include "display_handler.hpp"

display_handler::display_handler(const uint8_t screen_width,
                                 const uint8_t screen_height,
                                 const int8_t reset_pin)
    : display(screen_width, screen_height, &Wire, reset_pin),
      current_values({0.0, 0.0, 0.0}),
      sensor_units({"C", "ppm", "hpa"}),
      display_index(0),
      check_time_rotate(0),
      rotation_time(0),
      rotate(false),
      check_time_display(0),
      display_off_time(0),
      first_rotation(true) {
  set_temperature_unit(sensor_units[0]);
}

display_handler::~display_handler() { empty_display(); }

bool display_handler::initialize_display(const uint8_t screen_i2c_address) {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  return display.begin(SSD1306_SWITCHCAPVCC, screen_i2c_address);
}

void display_handler::empty_display() {
  rotate = false;
  // Clear the buffer
  display.clearDisplay();
  // Display empty buffer
  display.display();
}

void display_handler::change_rotation_time(const unsigned int seconds) {
  if (rotation_time != seconds) rotation_time = seconds;
}

void display_handler::start_rotation() {
  check_time_rotate = millis();
  check_time_display = millis();
  rotate = true;
  display_index = 0;
}

void display_handler::set_sensor_values(const sensor_values &values) {
  auto values_copy = values;
  // if unit is °F convert value
  if (sensor_units[0].find("F") != std::string::npos)
    values_copy[0] = values_copy[0] * 1.8 + 32;
  if (sensor_units[2].find("bar") != std::string::npos)
    values_copy[2] = values_copy[2] / 1000;
  current_values = values_copy;
}

void display_handler::set_units(const units &units_) {
  auto units_copy = units_;
  set_temperature_unit(units_copy[0]);
  if (sensor_units == units_copy) return;
  sensor_units = units_copy;
  // convert values for new units
  set_sensor_values(current_values);
}

void display_handler::print_value(const double value, const std::string &unit) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);  // Start at top-left corner
  display.cp437(true);
  display.print(value);
  display.print(unit.c_str());
  display.display();
}

void display_handler::display_sensor_value() {
  if (display_index >= 3) display_index = 0;

  print_value(current_values[display_index], sensor_units[display_index]);
}

void display_handler::check_if_off() {
  auto current_time = millis();
  if ((current_time - check_time_display) >= (display_off_time * 60 * 1000)) {
    check_time_display = current_time;
    empty_display();
  }
}

void display_handler::sensor_values_display_rotation() {
  check_if_off();

  if (rotate == false) return;

  auto current_time = millis();
  if (((current_time - check_time_rotate) < (rotation_time * 1000)) &&
      !first_rotation)
    return;
  check_time_rotate = current_time;
  first_rotation = false;
  display_sensor_value();
  ++display_index;
}

void display_handler::set_display_off_time(const unsigned int minutes) {
  if (display_off_time != minutes) display_off_time = minutes;
}

void display_handler::set_temperature_unit(std::string &temp_unit) {
  const char degree_char = 248;
  const std::string degree = std::string(&degree_char);
  temp_unit = degree + temp_unit;
}
