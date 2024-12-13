#include "wolf/controller/svas3_heating.hpp"
#include "fixture_controller_ventilation_mock.hpp"
#include "gtest/gtest.h"

using minutes = std::chrono::minutes;
using seconds = std::chrono::seconds;

std::shared_ptr<wolf::controller::svas3_heating> make_svas3(
    wolf::controller::controller& controller) {
  auto& control = controller.controller_;
  auto ventilation =
      std::make_shared<fixture_controller_ventilation_mock>(control);
  auto svas3 = std::make_shared<wolf::controller::svas3_heating>(ventilation);
  return svas3;
}

void set_active_inactive(
    std::shared_ptr<wolf::controller::svas3_heating>& svas3, const bool state) {
  EXPECT_NO_THROW(svas3->set_control_loop_active(state));
  bool active;
  EXPECT_NO_THROW(active = svas3->control_loop_is_active());
  EXPECT_EQ(active, state);
}

void set_limits(std::shared_ptr<wolf::controller::svas3_heating>& svas3) {
  wolf::controller::limits::temperature limits;
  limits.min = 22.f;
  limits.reference = 25.f;
  EXPECT_NO_THROW(svas3->set_limits(limits));
}

void set_reset_outputs(std::shared_ptr<wolf::controller::svas3_heating>& svas3,
                       const float absolute, const float relative,
                       const float temperature, const bool state_ventilation,
                       const bool state_heating) {
  EXPECT_NO_THROW(svas3->control(absolute, relative, temperature, true));
  auto outputs = svas3->outputs_are_set();
  EXPECT_EQ(outputs.ventilation, state_ventilation);
  EXPECT_EQ(outputs.heating, state_heating);
}

TEST(TestSVAS3Heating, SetAndCheckActive) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  set_active_inactive(svas3, true);
}

TEST(TestSVAS3Heating, SetAndCheckInactive) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  set_active_inactive(svas3, true);
  set_active_inactive(svas3, false);
}

TEST(TestSVAS3Heating, SetAndGetLimits) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  set_limits(svas3);
  wolf::controller::limits::temperature limits;
  EXPECT_NO_THROW(limits = svas3->get_limits());
  EXPECT_EQ(limits.min, 22.f);
  EXPECT_EQ(limits.reference, 25.f);
}

TEST(TestSVAS3Heating, SetOutputsErrorNotActive) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  set_reset_outputs(svas3, 10.5f, 70.f, 22.5f, false, false);
}

TEST(TestSVAS3Heating, SetOutputsVentilation) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 10.5f, 60.f, 30.f, true, false);
}

TEST(TestSVAS3Heating, SetResetOutputsVentilation) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 10.5f, 60.f, 30.f, true, false);
  set_reset_outputs(svas3, 5.5f, 60.f, 30.f, false, false);
}

TEST(TestSVAS3Heating, SetOutputsHeating) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.5f, 60.f, 20.f, false, true);
}

TEST(TestSVAS3Heating, SetResetOutputsHeating) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.5f, 60.f, 20.f, false, true);
  set_reset_outputs(svas3, 5.5f, 60.f, 30.f, false, false);
}

TEST(TestSVAS3Heating, SetOutputsHeatingToReference) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.5f, 60.f, 23.f, false, false);
  set_reset_outputs(svas3, 5.f, 60.f, 20.f, false, true);
  set_reset_outputs(svas3, 5.f, 60.f, 23.f, false, true);
  set_reset_outputs(svas3, 5.f, 60.f, 25.f, false, false);
}

TEST(TestSVAS3Heating, SetResetOutputsHeatingResetHeating) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.5f, 60.f, 23.f, false, false);
  set_reset_outputs(svas3, 5.f, 60.f, 20.f, false, true);
  set_reset_outputs(svas3, 5.f, 60.f, 23.f, false, true);
  svas3->reset_heating();
  set_reset_outputs(svas3, 5.f, 60.f, 23.f, false, false);
}

TEST(TestSVAS3Heating, SetLoopWithActiveOutputInactiveVentilation) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 10.5f, 60.f, 30.f, true, false);
  svas3->set_control_loop_active(false);
  auto outputs = svas3->outputs_are_set();
  EXPECT_FALSE(outputs.ventilation);
  EXPECT_FALSE(outputs.heating);
}

TEST(TestSVAS3Heating, SetLoopWithActiveOutputInactiveHeating) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.f, 60.f, 20.f, false, true);
  svas3->set_control_loop_active(false);
  auto outputs = svas3->outputs_are_set();
  EXPECT_FALSE(outputs.ventilation);
  EXPECT_FALSE(outputs.heating);
}

TEST(TestSVAS3Heating, SetResetOutputsHeatingSetVentilation) {
  wolf::controller::controller controller;
  auto svas3 = make_svas3(controller);
  svas3->set_control_loop_active(true);
  set_limits(svas3);
  set_reset_outputs(svas3, 5.f, 60.f, 20.f, false, true);
  set_reset_outputs(svas3, 5.f, 60.f, 30.f, false, false);
  set_reset_outputs(svas3, 10.5f, 60.f, 30.f, true, false);
}
