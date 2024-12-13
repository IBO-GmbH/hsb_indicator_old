#include "mold/median_calculator.hpp"
#include "gtest/gtest.h"

namespace median_calculator_test {

using clock = std::chrono::system_clock;
using timepoint = clock::time_point;
using minutes = std::chrono::minutes;

mold::config make_config() {
  mold::config config;
  config.median_.evaluation_humidity_median_time = 60;
  config.median_.use_median = true;
  return config;
}

static void calculate(mold::median_calculator& calculator,
                      const float calculation_value,
                      const timepoint& time_value, const float check_value) {
  float value = calculator.calculate_median(calculation_value, time_value);
  EXPECT_EQ(value, check_value);
}

TEST(TestMedianCalculator, CalculateOneValue) {
  auto config = make_config();
  mold::median_calculator calculator(config);
  const float check_value = 30.f;
  auto now = clock::now();
  calculate(calculator, check_value, now, check_value);
}

TEST(TestMedianCalculator, CalculateOnlyOldValues) {
  auto config = make_config();
  mold::median_calculator calculator(config);
  const float value_1 = 30.f;
  auto time = clock::now();
  calculate(calculator, value_1, time, value_1);
  const float value_2 = 20.f;
  time += minutes(65);
  calculate(calculator, value_2, time, value_2);
  const float value_3 = 50.f;
  time += minutes(65);
  calculate(calculator, value_3, time, value_3);
}

TEST(TestMedianCalculator, CalculateNewValues) {
  auto config = make_config();
  mold::median_calculator calculator(config);
  const float value_1 = 30.f;
  auto time = clock::now();
  calculate(calculator, value_1, time, value_1);
  const float value_2 = 20.f;
  time += minutes(5);
  calculate(calculator, value_2, time, ((value_2 + value_1) / 2));
  const float value_3 = 50.f;
  time += minutes(5);
  calculate(calculator, value_3, time, value_1);
}
}  // namespace median_calculator_test
