#ifndef WOLF_CONTROLLER_CALCULATOR_HPP
#define WOLF_CONTROLLER_CALCULATOR_HPP

namespace wolf {

namespace controller {

class calculator {
 public:
  static float calculate_absolute_humidity(const float temperature,
                                           const float humidity);
};
}
}

#endif  // WOLF_CONTROLLER_CALCULATOR_HPP
