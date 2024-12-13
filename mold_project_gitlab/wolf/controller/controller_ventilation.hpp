#ifndef WOLF_CONTROLLER_CONTROLLER_VENTILATION_HPP
#define WOLF_CONTROLLER_CONTROLLER_VENTILATION_HPP

#include "controller.hpp"
#include "log/logger.hpp"
#include "on_off_control.hpp"
#include "type.hpp"

#include <boost/signals2.hpp>

namespace wolf {

namespace controller {

class controller_ventilation {
 public:
  controller_ventilation(controller::control_array &control);
  void set_difference_humidity(const float difference_humidity);
  float get_difference_humidity();
  virtual void control(const float current_absolute_humidity,
                       const float current_relative_humidity);
  bool output_is_set();
  void reset_ventilation();
  void set_absolute_humidity_thresholds(const thresholds &absolute_humidity);
  void set_relative_humidity_thresholds(const thresholds &relative_humidity);
  virtual ~controller_ventilation() = default;

  boost::signals2::signal<void()> signal_thresholds_changed;

 private:
  bool control(const float current_value, const type &type_);
  void check_lower_threshold();
  on_off_control &get_on_of_control(const type &type_);
  void save_lower_threshold(const float lower);
  bool compare_thresholds(const thresholds &old, const thresholds &new_);
  void set_thresholds(const thresholds &thresholds_, const type &type_);

 protected:
  void set_output(const bool output_state);

 private:
  logging::logger m_logger;
  controller::control_array &m_control;
  bool m_output;
  float m_difference_humidity;
  float m_min;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_CONTROLLER_VENTILATION_HPP
