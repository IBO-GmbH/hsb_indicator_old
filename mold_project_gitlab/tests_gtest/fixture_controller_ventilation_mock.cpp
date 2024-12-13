#include "fixture_controller_ventilation_mock.hpp"

fixture_controller_ventilation_mock::fixture_controller_ventilation_mock(
    wolf::controller::controller::control_array &control)
    : wolf::controller::controller_ventilation(control) {}

void fixture_controller_ventilation_mock::control(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  if (current_absolute_humidity > 10) {
    set_output(true);
    return;
  }
  set_output(false);
}
