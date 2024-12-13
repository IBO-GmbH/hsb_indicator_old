#include "wolf/controller/controller_ventilation.hpp"
#include "gtest/gtest.h"

namespace controller_ventilation_test {

static logging::logger test_logger("controller_ventilation_test");

struct limits {
  wolf::controller::thresholds absolute;
  wolf::controller::thresholds relative;
};

void set_controller(wolf::controller::controller_ventilation& ventilation,
                    const limits& limits_) {
  ventilation.set_absolute_humidity_thresholds(limits_.absolute);
  ventilation.set_relative_humidity_thresholds(limits_.relative);
}

void set_reset_output(wolf::controller::controller_ventilation& ventilation,
                      const float difference, const float current_absolute,
                      const float current_relative, const bool set) {
  ventilation.set_difference_humidity(difference);
  EXPECT_NO_THROW(ventilation.control(current_absolute, current_relative));
  EXPECT_EQ(ventilation.output_is_set(), set);
}

TEST(TestControllerVentilation, SetAndGetDifferenceHumidity) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  EXPECT_NO_THROW(ventilation.set_difference_humidity(3.f));
  float difference;
  EXPECT_NO_THROW(difference = ventilation.get_difference_humidity());
  EXPECT_EQ(difference, 3.f);
}

TEST(TestControllerVentilation, ControlVentilationNoError) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  EXPECT_NO_THROW(ventilation.control(10.3f, 80.f));
}

TEST(TestControllerVentilation, ControlVentilationSetOutput) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 80.f, true);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputAbsoluteUpper) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 30.3f, 80.f, false);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputAbsoluteLower) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 4.3f, 80.f, false);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputRelativeUpper) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 98.f, false);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputRelativeLower) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 50.f, false);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputBothUpper) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 30.3f, 98.f, false);
}

TEST(TestControllerVentilation, ControlVentilationResetOutputBothLower) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 4.3f, 50.f, false);
}

TEST(TestControllerVentilation, ControlVentilationSetResetOutput) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 80.f, true);
  set_reset_output(ventilation, 3.f, 4.3f, 50.f, false);
}

TEST(TestControllerVentilation, ControlVentilationSetResetSetOutput) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 80.f, true);
  set_reset_output(ventilation, 3.f, 4.3f, 50.f, false);
  set_reset_output(ventilation, 3.f, 15.3f, 70.f, true);
}

TEST(TestControllerVentilation, ControlVentilationSetResetSetOutputDifference) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 59.f, false);
  set_reset_output(ventilation, 3.f, 10.3f, 60.f, false);
  set_reset_output(ventilation, 3.f, 10.3f, 61.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 80.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 59.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 58.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 57.f, false);
  set_reset_output(ventilation, 3.f, 10.3f, 56.f, false);
  set_reset_output(ventilation, 3.f, 10.3f, 50.f, false);
}

TEST(TestControllerVentilation,
     ControlVentilationSetResetOutputResetVentilation) {
  wolf::controller::controller controller;
  wolf::controller::controller_ventilation ventilation(controller.controller_);
  set_controller(ventilation, {{30.f, 5.f}, {90.f, 60.f}});
  set_reset_output(ventilation, 3.f, 10.3f, 80.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 60.f, true);
  set_reset_output(ventilation, 3.f, 10.3f, 58.f, true);
  ventilation.reset_ventilation();
  set_reset_output(ventilation, 3.f, 10.3f, 58.f, false);
  set_reset_output(ventilation, 3.f, 10.3f, 50.f, false);
}
}  // namespace controller_ventilation_test
