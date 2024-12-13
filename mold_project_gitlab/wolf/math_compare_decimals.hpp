#ifndef WOLF_MATH_COMPARE_DECIMALS_HPP
#define WOLF_MATH_COMPARE_DECIMALS_HPP

#include <cmath>
#include <limits>

namespace wolf {
namespace math {
static bool compare_floats(const float first, const float second) {
  return std::abs(first - second) <= std::numeric_limits<float>::epsilon();
};

static bool compare_doubles(const double first, const double second) {
  return std::abs(first - second) <= std::numeric_limits<double>::epsilon();
};
}  // namespace math
}  // namespace wolf

#endif  // WOLF_MATH_COMPARE_DECIMALS_HPP
