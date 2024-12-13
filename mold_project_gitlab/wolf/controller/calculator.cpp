#include "calculator.hpp"

#include <cmath>

float wolf::controller::calculator::calculate_absolute_humidity(
    const float temperature, const float humidity) {
  float absolute_humitdity =
      216.7f * ((humidity / 100.f) * 6.112f *
                std::exp(17.62f * temperature / (243.12f + temperature)) /
                (273.15f + temperature));
  return absolute_humitdity;
}
