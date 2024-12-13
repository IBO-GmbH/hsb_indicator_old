#include "wolf/controller/svas4_air_quality.hpp"
#include "gtest/gtest.h"

void set_active_inactive(wolf::controller::svas4_air_quality& svas4,
                         const bool state) {
  EXPECT_NO_THROW(svas4.set_control_loop_active(state));
  bool active;
  EXPECT_NO_THROW(active = svas4.control_loop_is_active());
  EXPECT_EQ(active, state);
}

struct limits {
  wolf::controller::thresholds co2;
  wolf::controller::thresholds sprout_value;
  wolf::controller::thresholds voc;
};

void set_thresholds(wolf::controller::svas4_air_quality& air_quality,
                    const limits& limits_) {
  air_quality.set_co2_thresholds(limits_.co2);
  air_quality.set_sprout_value_thresholds(limits_.sprout_value);
  air_quality.set_voc_thresholds(limits_.voc);
}

void set_differences(wolf::controller::svas4_air_quality& svas4) {
  svas4.set_co2_difference(100.f);
  svas4.set_sprout_value_difference(10.f);
  svas4.set_voc_difference(20.f);
}

void set_reset_output(wolf::controller::svas4_air_quality& air_quality,
                      const float co2, const float sprout_value,
                      const float voc, const bool set) {
  EXPECT_NO_THROW(air_quality.control(co2, sprout_value, voc, true));
  EXPECT_EQ(air_quality.output_is_set(), set);
}

TEST(TestSVAS4AirQuality, SetAndCheckActive) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_active_inactive(svas4, true);
}

TEST(TestSVAS4AirQuality, SetAndCheckInactive) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_active_inactive(svas4, true);
  set_active_inactive(svas4, false);
}

TEST(TestSVAS4AirQuality, SetOutputErrorNotActive) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_reset_output(svas4, 2000.f, 0.7f, 65.7f, false);
}

TEST(TestSVAS4AirQuality, SetOutputCO2) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, SetOutputSproutValue) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, SetOutputVOC) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputCO2) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, ResetOutputSproutValue) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, ResetOutputVOC) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, ResetOutputCO2SetSproutValue) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputCO2SetVOC) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputSproutValueSetCO2) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputSproutValueSetVOC) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputVOCSetCO2) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, ResetOutputVOCSetSproutValue) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
}

TEST(TestSVAS4AirQuality, SetOutputAllThree) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 80.f, 300.f, true);
}

TEST(TestSVAS4AirQuality, SetResetOutputAllThree) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 80.f, 300.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, SetResetOutputAllThreeAfterAnother) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, SetLoopWithActiveOutputInactiveCO2) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_active_inactive(svas4, false);
  EXPECT_FALSE(svas4.output_is_set());
}

TEST(TestSVAS4AirQuality, SetLoopWithActiveOutputInactiveSproutValue) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 80.f, 50.f, true);
  set_active_inactive(svas4, false);
  EXPECT_FALSE(svas4.output_is_set());
}

TEST(TestSVAS4AirQuality, SetLoopWithActiveOutputInactiveVOC) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 300.f, true);
  set_active_inactive(svas4, false);
  EXPECT_FALSE(svas4.output_is_set());
}

TEST(TestSVAS4AirQuality, ResetOutputCO2WithBorder) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 2000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 2100.f, 50.f, 50.f, true);
  set_reset_output(svas4, 2000.f, 50.f, 50.f, true);
  set_reset_output(svas4, 1990.f, 50.f, 50.f, true);
  set_reset_output(svas4, 1950.f, 50.f, 50.f, true);
  set_reset_output(svas4, 1900.f, 50.f, 50.f, false);
  set_reset_output(svas4, 1890.f, 50.f, 50.f, false);
  set_reset_output(svas4, 1000.f, 50.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, ResetOutputSproutValueWithBorder) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 61.f, 50.f, true);
  set_reset_output(svas4, 50.f, 60.f, 50.f, true);
  set_reset_output(svas4, 50.f, 59.f, 50.f, true);
  set_reset_output(svas4, 50.f, 55.f, 50.f, true);
  set_reset_output(svas4, 50.f, 51.f, 50.f, true);
  set_reset_output(svas4, 50.f, 50.f, 50.f, false);
  set_reset_output(svas4, 50.f, 49.f, 50.f, false);
  set_reset_output(svas4, 50.f, 45.f, 50.f, false);
}

TEST(TestSVAS4AirQuality, ResetOutputVOCWithBorder) {
  wolf::controller::controller controller;
  wolf::controller::svas4_air_quality svas4(controller.controller_);
  set_thresholds(svas4, {{10000.f, 1000.f}, {90.f, 60.f}, {1000.f, 100.f}});
  set_active_inactive(svas4, true);
  set_differences(svas4);
  set_reset_output(svas4, 50.f, 50.f, 200.f, true);
  set_reset_output(svas4, 50.f, 50.f, 100.f, true);
  set_reset_output(svas4, 50.f, 50.f, 99.f, true);
  set_reset_output(svas4, 50.f, 50.f, 95.f, true);
  set_reset_output(svas4, 50.f, 50.f, 90.f, true);
  set_reset_output(svas4, 50.f, 50.f, 85.f, true);
  set_reset_output(svas4, 50.f, 50.f, 81.f, true);
  set_reset_output(svas4, 50.f, 50.f, 80.f, false);
  set_reset_output(svas4, 50.f, 50.f, 79.f, false);
  set_reset_output(svas4, 50.f, 50.f, 70.f, false);
}
