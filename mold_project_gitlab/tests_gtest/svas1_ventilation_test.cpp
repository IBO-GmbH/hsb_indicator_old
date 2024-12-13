#include "wolf/controller/svas1_ventilation.hpp"
#include "fixture_controller_ventilation_mock.hpp"
#include "gtest/gtest.h"

void set_active_inactive(wolf::controller::svas1_ventilation& svas1,
                         const bool state) {
  EXPECT_NO_THROW(svas1.set_control_loop_active(state));
  bool active;
  EXPECT_NO_THROW(active = svas1.control_loop_is_active());
  EXPECT_EQ(active, state);
}

void set_reset_output(wolf::controller::svas1_ventilation& svas1,
                      const float absolute, const bool state) {
  EXPECT_NO_THROW(svas1.control(absolute, 50.f, true));
  EXPECT_EQ(svas1.output_is_set(), state);
}

TEST(TestSVAS1Ventilation, SetAndCheckActive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  set_active_inactive(svas1, true);
}

TEST(TestSVAS1Ventilation, SetAndCheckInactive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  set_active_inactive(svas1, true);
  set_active_inactive(svas1, false);
}

TEST(TestSVAS1Ventilation, SetOutputErrorNotActive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  set_reset_output(svas1, 15.5f, false);
}

TEST(TestSVAS1Ventilation, SetOutputNoError) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  svas1.set_control_loop_active(true);
  set_reset_output(svas1, 15.5f, true);
}

TEST(TestSVAS1Ventilation, ResetOutputNoError) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  svas1.set_control_loop_active(true);
  set_reset_output(svas1, 5.5f, false);
}

TEST(TestSVAS1Ventilation, SetResetOutput) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  svas1.set_control_loop_active(true);
  set_reset_output(svas1, 15.5f, true);
  set_reset_output(svas1, 5.5f, false);
}

TEST(TestSVAS1Ventilation, SetResetSetOutput) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  svas1.set_control_loop_active(true);
  set_reset_output(svas1, 15.5f, true);
  set_reset_output(svas1, 5.5f, false);
  set_reset_output(svas1, 15.5f, true);
}

TEST(TestSVAS1Ventilation, SetLoopWithActiveOutputInactive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas1_ventilation svas1(ventilation);
  svas1.set_control_loop_active(true);
  set_reset_output(svas1, 15.5f, true);
  svas1.set_control_loop_active(false);
  EXPECT_FALSE(svas1.output_is_set());
}
