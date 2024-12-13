#include "button_handler.hpp"

button_handler::button_handler(Button& button, display_handler& display_,
                               wifi_connection& wifi_,
                               const int multi_press_number_,
                               const int multi_press_delay_,
                               const int long_press_time_)
    : button_to_check(button),
      display(display_),
      wifi(wifi_),
      multi_press_number(multi_press_number_),
      multi_press_delay(multi_press_delay_),
      long_press_time(long_press_time_),
      button_pressed(false),
      press_count(0) {}

button_handler::~button_handler() {}

button_handler::pressed_state button_handler::check_pressed_state(
    const double value, const std::string& unit) {
  if (button_to_check.wasPressed()) button_pressed = true;
  if (check_single_press()) return handle_single_press();
  if (check_long_press()) return handle_long_press();
  if (check_multi_press()) return handle_multi_press();
  return pressed_state::not_pressed;
}

bool button_handler::check_single_press() {
  return (button_to_check.releasedFor(multi_press_delay) && button_pressed);
}

bool button_handler::check_long_press() {
  return button_to_check.pressedFor(long_press_time * 1000);
}

bool button_handler::check_multi_press() {
  if (((millis() - button_to_check.lastChange()) < multi_press_delay) &&
      button_to_check.wasPressed()) {
    ++press_count;
    if (press_count >= multi_press_number) {
      press_count = 0;
      return true;
    }
  }
  return false;
}

button_handler::pressed_state button_handler::handle_single_press() {
  button_pressed = false;
  press_count = 0;
  return pressed_state::single_press;
}

button_handler::pressed_state button_handler::handle_long_press() {
  button_pressed = false;
  press_count = 0;
  return pressed_state::long_press;
}

button_handler::pressed_state button_handler::handle_multi_press() {
  button_pressed = false;
  return pressed_state::multi_press;
}
