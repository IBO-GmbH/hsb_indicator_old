#ifndef WOLF_CONTROLLER_SVAS1_VENTILATION_HPP
#define WOLF_CONTROLLER_SVAS1_VENTILATION_HPP

#include "controller_ventilation.hpp"
#include "log/logger.hpp"

namespace wolf {

namespace controller {

class svas1_ventilation {
 public:
  svas1_ventilation(
      std::shared_ptr<controller_ventilation> controller_ventilation_);
  void control(const float current_absolute_humidity,
               const float current_relative_humidity,
               const bool outdoor_temperature_right);
  bool output_is_set();
  void set_control_loop_active(const bool active_state);
  bool control_loop_is_active() const;
  void reset_ventilation();

 private:
  void set_output(const bool state);
  bool detect_change(const float current_absolute_humidity,
                     const float current_relative_humidity);
  bool handle_change(const float current_absolute_humidity,
                     const float current_relative_humidity);

 private:
  logging::logger m_logger;
  std::shared_ptr<controller_ventilation> m_ventilation;
  bool m_active;
  bool m_output;
  float m_old_relative_humidity;
  float m_old_absolute_humidity;
  bool m_settings_changed;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SVAS1_VENTILATION_HPP
