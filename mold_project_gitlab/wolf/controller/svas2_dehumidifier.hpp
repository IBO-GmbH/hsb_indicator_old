#ifndef WOLF_CONTROLLER_SVAS2_DEHUMIDIFIER_HPP
#define WOLF_CONTROLLER_SVAS2_DEHUMIDIFIER_HPP

#include "controller_ventilation.hpp"
#include "limits.hpp"
#include "log/logger.hpp"
#include "wolf/program_options_parser.hpp"

namespace wolf {

namespace controller {

class svas2_dehumidifier {
 public:
  struct outputs {
    bool ventilation;
    bool dehumidifier;
  };

  svas2_dehumidifier(
      std::shared_ptr<controller_ventilation> controller_ventilation_);
  void set_humidity_border(const float humidity_border);
  void set_humidity_difference(const float difference_humidity);
  float get_humidity_border();
  void control(const float current_absolute_humidity,
               const float current_relative_humidity,
               const bool outdoor_temperature_right);
  outputs outputs_are_set();
  void set_control_loop_active(const bool active_state);
  bool control_loop_is_active() const;
  void reset_ventilation();
  void reset_dehumidifier();

 private:
  void control_ventilation(const float current_absolute_humidity,
                           const float current_relative_humidity);
  void control_dehumidifier(const float current_absolute_humidity,
                            const float current_relative_humidity);
  void set_output_ventilation(const bool state);
  void set_output_dehumidifier(const bool state);
  bool detect_change(const float current_absolute_humidity,
                     const float current_relative_humidity);
  bool handle_change(const float current_absolute_humidity,
                     const float current_relative_humidity);

 private:
  logging::logger m_logger;
  std::shared_ptr<controller_ventilation> m_ventilation;
  float m_border;
  float m_difference;
  outputs m_outputs;
  bool m_active;
  float m_old_relative_humidity;
  float m_old_absolute_humidity;
  bool m_settings_changed;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SVAS2_DEHUMIDIFIER_HPP
