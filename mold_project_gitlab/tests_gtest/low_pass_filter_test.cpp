#include "gtest/gtest.h"
#include "wolf/low_pass_filter_creator.hpp"

wolf::low_pass_filter create() {
  wolf::low_pass_filter_creator creator(
      {0.25f, 2.f, 2.f * 600, std::chrono::minutes(15)});
  return creator.create();
}

wolf::low_pass_filter create_small() {
  wolf::low_pass_filter_creator creator(
      {0.5f, 0.005f, 0.005f * 600, std::chrono::minutes(15)});
  return creator.create();
}

TEST(LowPassFilter, TriggerValue) {
  auto filter = create();
  EXPECT_TRUE(filter.handle_value(50.f));
  EXPECT_FALSE(filter.handle_value(51.f));
  EXPECT_TRUE(filter.handle_value(70.f));
  EXPECT_FALSE(filter.handle_value(71.f));
  EXPECT_TRUE(filter.handle_value(30.f));
}

TEST(LowPassFilter, TriggerFilterConstant) {
  auto filter = create();
  // slowly rise by one, constant of 0.25 and value of 2 causes it to trigger at
  // 55/59.5
  EXPECT_TRUE(filter.handle_value(50.f));
  EXPECT_FALSE(filter.handle_value(51.f));
  EXPECT_FALSE(filter.handle_value(52.f));
  EXPECT_FALSE(filter.handle_value(53.f));
  EXPECT_FALSE(filter.handle_value(54.f));
  EXPECT_TRUE(filter.handle_value(55.f));
  EXPECT_FALSE(filter.handle_value(56.f));
  EXPECT_FALSE(filter.handle_value(57.f));
  EXPECT_FALSE(filter.handle_value(58.f));
  EXPECT_FALSE(filter.handle_value(59.f));
  EXPECT_TRUE(filter.handle_value(59.5f));
  // reset and rise even slower, now it triggers at 53 because of the constant
  EXPECT_TRUE(filter.handle_value(50.f));
  EXPECT_FALSE(filter.handle_value(51.f));
  EXPECT_FALSE(filter.handle_value(51.f));
  EXPECT_FALSE(filter.handle_value(51.f));
  EXPECT_FALSE(filter.handle_value(52.f));
  EXPECT_FALSE(filter.handle_value(52.f));
  EXPECT_FALSE(filter.handle_value(52.f));
  EXPECT_FALSE(filter.handle_value(53.f));
  EXPECT_TRUE(filter.handle_value(53.f));
  // because of last value and constant 7 is not enough raise to trigger
  EXPECT_FALSE(filter.handle_value(60.f));
  // another 5 is enough to trigger
  EXPECT_TRUE(filter.handle_value(65.f));
  // reset and stay at constant value over threshold, filter constant will
  // trigger after some cycles
  EXPECT_TRUE(filter.handle_value(50.f));
  EXPECT_FALSE(filter.handle_value(52.5f));
  EXPECT_FALSE(filter.handle_value(52.5f));
  EXPECT_FALSE(filter.handle_value(52.5f));
  EXPECT_FALSE(filter.handle_value(52.5f));
  EXPECT_FALSE(filter.handle_value(52.5f));
  EXPECT_TRUE(filter.handle_value(52.5f));
}

TEST(LowPassFilter, TriggerTime) {
  auto filter = create();
  auto now = std::chrono::steady_clock::now();
  EXPECT_TRUE(filter.handle_value(50.f, now));
  now += std::chrono::minutes(10);
  EXPECT_FALSE(filter.handle_value(50.f, now));
  now += std::chrono::minutes(10);
  EXPECT_TRUE(filter.handle_value(50.f, now));
}

TEST(LowPassFilter, TriggerTimeThreshold) {
  auto filter = create();
  auto now = std::chrono::steady_clock::now();
  EXPECT_TRUE(filter.handle_value(50.f, now));
  now += std::chrono::minutes(5);
  EXPECT_FALSE(filter.handle_value(52.f, now));
  now += std::chrono::minutes(8);
  // time is less than 15 min (max time diff) and value not big enough to
  // trigger diff
  EXPECT_TRUE(filter.handle_value(55.f, now));
}

TEST(LowPassFilter, TriggerValueSmall) {
  auto filter = create_small();
  EXPECT_TRUE(filter.handle_value(.10f));
  EXPECT_FALSE(filter.handle_value(.105f));
  EXPECT_TRUE(filter.handle_value(.11f));
  EXPECT_FALSE(filter.handle_value(.115f));
  EXPECT_TRUE(filter.handle_value(.12f));
}

TEST(LowPassFilter, TriggerFilterConstantSmall) {
  auto filter = create_small();
  // slowly rise by 0.001, constant of 0.5 and value of 0.005 causes it to
  // trigger at 0.106/0.112
  EXPECT_TRUE(filter.handle_value(.10f));
  EXPECT_FALSE(filter.handle_value(.101f));
  EXPECT_FALSE(filter.handle_value(.102f));
  EXPECT_FALSE(filter.handle_value(.103f));
  EXPECT_FALSE(filter.handle_value(.104f));
  EXPECT_FALSE(filter.handle_value(.105f));
  EXPECT_TRUE(filter.handle_value(.106f));
  EXPECT_FALSE(filter.handle_value(.107f));
  EXPECT_FALSE(filter.handle_value(.108f));
  EXPECT_FALSE(filter.handle_value(.109f));
  EXPECT_FALSE(filter.handle_value(.11f));
  EXPECT_FALSE(filter.handle_value(.111f));
  EXPECT_TRUE(filter.handle_value(.112f));
  // reset and rise even slower, now it triggers at 0.1055 because of the
  // constant
  EXPECT_TRUE(filter.handle_value(.10f));
  EXPECT_FALSE(filter.handle_value(.10025f));
  EXPECT_FALSE(filter.handle_value(.1005f));
  EXPECT_FALSE(filter.handle_value(.10075f));
  EXPECT_FALSE(filter.handle_value(.101f));
  EXPECT_FALSE(filter.handle_value(.10125f));
  EXPECT_FALSE(filter.handle_value(.1015f));
  EXPECT_FALSE(filter.handle_value(.10175f));
  EXPECT_FALSE(filter.handle_value(.102f));
  EXPECT_FALSE(filter.handle_value(.10225f));
  EXPECT_FALSE(filter.handle_value(.1025f));
  EXPECT_FALSE(filter.handle_value(.10275f));
  EXPECT_FALSE(filter.handle_value(.103f));
  EXPECT_FALSE(filter.handle_value(.10325f));
  EXPECT_FALSE(filter.handle_value(.1035f));
  EXPECT_FALSE(filter.handle_value(.10375f));
  EXPECT_FALSE(filter.handle_value(.104f));
  EXPECT_FALSE(filter.handle_value(.10425f));
  EXPECT_FALSE(filter.handle_value(.1045f));
  EXPECT_FALSE(filter.handle_value(.10475f));
  EXPECT_FALSE(filter.handle_value(.105f));
  EXPECT_FALSE(filter.handle_value(.10525f));
  EXPECT_TRUE(filter.handle_value(.1055f));
  // because of last value and constant 0.0055 is not enough raise to trigger
  EXPECT_FALSE(filter.handle_value(.111f));
  // another 0.004 is enough to trigger
  EXPECT_TRUE(filter.handle_value(.115f));
  // reset and stay at constant value over threshold, filter constant will
  // trigger after some cycles
  EXPECT_TRUE(filter.handle_value(.1f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_FALSE(filter.handle_value(.10501f));
  EXPECT_TRUE(filter.handle_value(.10501f));
}

TEST(LowPassFilter, TriggerTimeSmall) {
  auto filter = create_small();
  auto now = std::chrono::steady_clock::now();
  EXPECT_TRUE(filter.handle_value(.1f, now));
  now += std::chrono::minutes(10);
  EXPECT_FALSE(filter.handle_value(.1f, now));
  now += std::chrono::minutes(10);
  EXPECT_TRUE(filter.handle_value(.1f, now));
}

TEST(LowPassFilter, TriggerTimeThresholdSmall) {
  auto filter = create_small();
  auto now = std::chrono::steady_clock::now();
  EXPECT_TRUE(filter.handle_value(.1f, now));
  now += std::chrono::minutes(5);
  EXPECT_FALSE(filter.handle_value(.105f, now));
  now += std::chrono::minutes(8);
  // time is less than 15 min (max time diff) and value not big enough to
  // trigger diff
  EXPECT_TRUE(filter.handle_value(.107f, now));
}
