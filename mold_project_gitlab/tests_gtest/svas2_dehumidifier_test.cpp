#include "wolf/controller/svas2_dehumidifier.hpp"
#include "fixture_controller_ventilation_mock.hpp"
#include "gtest/gtest.h"

void set_active_inactive(wolf::controller::svas2_dehumidifier& svas2,
                         const bool state) {
  EXPECT_NO_THROW(svas2.set_control_loop_active(state));
  bool active;
  EXPECT_NO_THROW(active = svas2.control_loop_is_active());
  EXPECT_EQ(active, state);
}

void set_reset_outputs(wolf::controller::svas2_dehumidifier& svas2,
                       const float absolute, const float relative,
                       const bool state_ventilation,
                       const bool state_dehumidifier) {
  EXPECT_NO_THROW(svas2.control(absolute, relative, true));
  auto outputs = svas2.outputs_are_set();
  EXPECT_EQ(outputs.ventilation, state_ventilation);
  EXPECT_EQ(outputs.dehumidifier, state_dehumidifier);
}

TEST(TestSVAS2Dehumidifier, SetAndCheckActive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  set_active_inactive(svas2, true);
}

TEST(TestSVAS2Dehumidifier, SetAndCheckInactive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  set_active_inactive(svas2, true);
  set_active_inactive(svas2, false);
}

TEST(TestSVAS2Dehumidifier, SetAndGetBordersNoError) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  EXPECT_NO_THROW(svas2.set_humidity_border(70.f));
  svas2.set_humidity_difference(5.f);
  auto border = svas2.get_humidity_border();
  EXPECT_EQ(border, 70.f);
}

TEST(TestSVAS2Dehumidifier, SetOutputsErrorNotActive) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  set_reset_outputs(svas2, 10.5f, 70.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetOutputsVentilation) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 60.f, true, false);
}

TEST(TestSVAS2Dehumidifier, SetResetOutputsVentilation) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 60.f, true, false);
  set_reset_outputs(svas2, 5.5f, 60.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetResetOutputsVentilationNearBorder) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 69.f, true, false);
  set_reset_outputs(svas2, 5.5f, 60.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetOutputsDehumidifier) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 80.f, false, true);
}

TEST(TestSVAS2Dehumidifier, SetOutputsDehumidifierAtBorder) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 70.f, false, true);
}

TEST(TestSVAS2Dehumidifier, SetOutputsDehumidifierNoResetBorder) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 70.f, false, true);
}

TEST(TestSVAS2Dehumidifier,
     SetOutputsDehumidifierNoResetBorderMinusDifference) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 5.5f, 65.f, false, false);
  set_reset_outputs(svas2, 5.5f, 69.f, false, false);
  set_reset_outputs(svas2, 5.5f, 70.f, false, true);
  set_reset_outputs(svas2, 5.5f, 69.f, false, true);
  set_reset_outputs(svas2, 5.5f, 65.f, false, true);
  set_reset_outputs(svas2, 5.5f, 54.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetResetOutputsDehumidifier) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 5.5f, 70.f, false, true);
  set_reset_outputs(svas2, 5.5f, 54.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetOutputsVentilationAfterDehumidifier) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 65.f, true, false);
  set_reset_outputs(svas2, 10.5f, 70.f, false, true);
  set_reset_outputs(svas2, 10.5f, 65.f, false, true);
  set_reset_outputs(svas2, 10.5f, 64.f, true, false);
  set_reset_outputs(svas2, 5.5f, 54.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetResetOutputsDehumidifierResetDehumidifier) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 5.5f, 68.f, false, false);
  set_reset_outputs(svas2, 5.5f, 75.f, false, true);
  set_reset_outputs(svas2, 5.5f, 68.f, false, true);
  svas2.reset_dehumidifier();
  set_reset_outputs(svas2, 5.5f, 68.f, false, false);
}

TEST(TestSVAS2Dehumidifier, SetLoopWithActiveOutputInactiveVentilation) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 60.f, true, false);
  svas2.set_control_loop_active(false);
  auto outputs = svas2.outputs_are_set();
  EXPECT_FALSE(outputs.ventilation);
  EXPECT_FALSE(outputs.dehumidifier);
}

TEST(TestSVAS2Dehumidifier, SetLoopWithActiveOutputInactiveDehumidifier) {
  wolf::controller::controller controller;
  auto ventilation = std::make_shared<fixture_controller_ventilation_mock>(
      controller.controller_);
  wolf::controller::svas2_dehumidifier svas2(ventilation);
  svas2.set_control_loop_active(true);
  svas2.set_humidity_border(70.f);
  svas2.set_humidity_difference(5.f);
  set_reset_outputs(svas2, 10.5f, 80.f, false, true);
  svas2.set_control_loop_active(false);
  auto outputs = svas2.outputs_are_set();
  EXPECT_FALSE(outputs.ventilation);
  EXPECT_FALSE(outputs.dehumidifier);
}
