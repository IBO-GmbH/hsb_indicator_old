#ifndef WOLF_MATH_MAP_HPP
#define WOLF_MATH_MAP_HPP

namespace wolf {
namespace math {

template <typename value_type>
value_type map(const value_type &value, const value_type &value_min,
               const value_type &value_max, const value_type &out_min,
               const value_type &out_max) {
  return ((value - value_min) * (out_max - out_min)) / (value_max - value_min) +
         out_min;
}
}
}

#endif  // WOLF_MATH_MAP_HPP
