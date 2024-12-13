#ifndef WOLF_CONTROLLER_SVAS3_HEATING_HPP
#define WOLF_CONTROLLER_SVAS3_HEATING_HPP

#include "controller_ventilation.hpp"
#include "limits.hpp"
#include "log/logger.hpp"
#include "on_off_control.hpp"

namespace wolf {

namespace controller {

class svas3_heating {
 public:
  struct outputs {
    bool ventilation;
    bool heating;
  };

  svas3_heating(
      std::shared_ptr<controller_ventilation> controller_ventilation_);
  void set_limits(const limits::temperature &limits);
  limits::temperature get_limits();
  void control(const float current_absolute_humidity,
               const float current_relative_humidity,
               const float current_temperature,
               const bool outdoor_temperature_right);
  outputs outputs_are_set();
  void set_control_loop_active(const bool active_state);
  bool control_loop_is_active() const;
  void reset_ventilation();
  void reset_heating();

 private:
  void control_ventilation(const float current_absolute_humidity,
                           const float current_relative_humidity);
  bool check_temperature(const float current_temperature);
  void control_heating(const float current_temperature);
  void set_output_ventilation(const bool state);
  void set_output_heating(const bool state);
  bool detect_change(const float current_absolute_humidity,
                     const float current_relative_humidity,
                     const float current_temperature);
  bool handle_change(const float current_absolute_humidity,
                     const float current_relative_humidity,
                     const float current_temperature);

 private:
  logging::logger m_logger;
  std::shared_ptr<controller_ventilation> m_ventilation;
  limits::temperature m_limits;
  outputs m_outputs;
  bool m_active;
  float m_old_relative_humidity;
  float m_old_absolute_humidity;
  float m_old_temperature;
  bool m_settings_changed;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SVAS3_HEATING_HPP
