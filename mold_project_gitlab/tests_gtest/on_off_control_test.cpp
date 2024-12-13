#include "wolf/controller/on_off_control.hpp"
#include "gtest/gtest.h"

void set_thresholds(wolf::controller::on_off_control& controller, float upper,
                    float lower) {
  controller.set_thresholds({upper, lower});
}

void set_controller(wolf::controller::on_off_control& controller,
                    const float value) {
  EXPECT_NO_THROW(controller.control_output(value));
  bool is_set;
  EXPECT_NO_THROW(is_set = controller.output_is_set());
  EXPECT_TRUE(is_set);
}

void reset_controller(wolf::controller::on_off_control& controller,
                      const float value) {
  EXPECT_NO_THROW(controller.control_output(value));
  bool is_set = false;
  EXPECT_NO_THROW(is_set = controller.output_is_set());
  EXPECT_FALSE(is_set);
}

TEST(TestOnOffControl, SetAndGetThresholds) {
  wolf::controller::on_off_control new_controller;
  EXPECT_NO_THROW(new_controller.set_thresholds({10.5f, 0.5f}));
  wolf::controller::thresholds thresholds;
  EXPECT_NO_THROW(thresholds = new_controller.get_thresholds());
  EXPECT_EQ(thresholds.upper, 10.5f);
  EXPECT_EQ(thresholds.lower, 0.5f);
}

TEST(TestOnOffControl, ControlOutputNoError) {
  wolf::controller::on_off_control new_controller;
  EXPECT_NO_THROW(new_controller.control_output(4.5f));
}

TEST(TestOnOffControl, ControlOutputSetOutput) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
}

TEST(TestOnOffControl, ControlOutputOutputKeepSet) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  set_controller(new_controller, 10.4f);
  set_controller(new_controller, 0.6f);
}

TEST(TestOnOffControl, ControlOutputResetOutputUpEqual) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 10.5f);
}

TEST(TestOnOffControl, ControlOutputResetOutputUpGreater) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 15.5f);
}

TEST(TestOnOffControl, ControlOutputResetOutputLowEqual) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 0.5f);
}

TEST(TestOnOffControl, ControlOutputResetOutputLowLess) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 0.2f);
}

TEST(TestOnOffControl, ControlOutputOutputKeepResetUp) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 15.4f);
  reset_controller(new_controller, 20.2f);
}

TEST(TestOnOffControl, ControlOutputOutputKeepResetLow) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 0.4f);
  reset_controller(new_controller, 0.2f);
}

TEST(TestOnOffControl, ControlOutputOutputKeepReset) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 15.4f);
  reset_controller(new_controller, 0.2f);
}

TEST(TestOnOffControl, ControlOutputOutputResetSet) {
  wolf::controller::on_off_control new_controller;
  set_thresholds(new_controller, 10.5f, 0.5f);
  set_controller(new_controller, 4.5f);
  reset_controller(new_controller, 15.4f);
  set_controller(new_controller, 8.2f);
}
