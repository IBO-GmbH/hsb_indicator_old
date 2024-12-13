#ifndef TESTS_GTEST_FIXTURE_CONTROLLER_VENTILATION_MOCK_HPP
#define TESTS_GTEST_FIXTURE_CONTROLLER_VENTILATION_MOCK_HPP

#include "wolf/controller/controller_ventilation.hpp"

class fixture_controller_ventilation_mock
    : public wolf::controller::controller_ventilation {
 public:
  fixture_controller_ventilation_mock(
      wolf::controller::controller::control_array &control);
  void control(const float current_absolute_humidity,
               const float current_relative_humidity) override;
};

#endif  // TESTS_GTEST_FIXTURE_CONTROLLER_VENTILATION_MOCK_HPP
