#ifndef WOLF_MATH_CLAMP_HPP
#define WOLF_MATH_CLAMP_HPP

#include <algorithm>

namespace wolf {
namespace math {

template <typename value_type>
value_type clamp(const value_type &value, const value_type &value_min,
                 const value_type &value_max) {
  return std::min<value_type>(value_max,
                              std::max<value_type>(value_min, value));
}
}
}

#endif  // WOLF_MATH_CLAMP_HPP
