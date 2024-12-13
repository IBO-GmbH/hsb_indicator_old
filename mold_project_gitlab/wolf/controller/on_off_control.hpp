#ifndef WOLF_CONTROLLER_ON_OFF_CONTROL_HPP
#define WOLF_CONTROLLER_ON_OFF_CONTROL_HPP

#include "log/logger.hpp"
#include "on_off_state.hpp"

namespace wolf {

namespace controller {

class on_off_control {
 public:
  on_off_control();
  void set_thresholds(const thresholds& thresholds);
  thresholds get_thresholds() const;
  void control_output(const float sensor_value);
  bool output_is_set() const;

 private:
  bool check_value(const float sensor_value);
  void set_output(bool output_state);

 private:
  friend std::ostream& operator<<(std::ostream& out,
                                  const on_off_control& to_print);

  logging::logger m_logger;
  on_off_state m_on_off_values;
};

std::ostream& operator<<(std::ostream& out, const on_off_control& to_print);
}
}

#endif  // WOLF_CONTROLLER_ON_OFF_CONTROL_HPP
