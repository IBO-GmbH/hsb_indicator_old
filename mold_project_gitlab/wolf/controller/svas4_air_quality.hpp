#ifndef WOLF_CONTROLLER_SVAS4_AIR_QUALITY_HPP
#define WOLF_CONTROLLER_SVAS4_AIR_QUALITY_HPP

#include "controller.hpp"
#include "log/logger.hpp"
#include "type.hpp"

namespace wolf {

namespace controller {

class svas4_air_quality {
 public:
  svas4_air_quality(controller::control_array &control);
  void control(const float current_co2, const float current_sprout_value,
               const float current_voc, const bool outdoor_temperature_right);
  void set_co2_thresholds(const thresholds &co2);
  void set_co2_difference(const float difference);
  void set_sprout_value_thresholds(const thresholds &sprout_value);
  void set_sprout_value_difference(const float difference);
  void set_voc_thresholds(const thresholds &voc);
  void set_voc_difference(const float difference);
  bool output_is_set();
  void set_control_loop_active(const bool active_state);
  bool control_loop_is_active() const;

 private:
  bool control(const float current_value, const type &type_) const;
  on_off_control &get_on_of_control(const type &type_) const;
  bool check_control(const float current_co2, const float current_sprout_value,
                     const float current_voc) const;
  void set_output(const bool state);
  void set_thresholds(const thresholds &thresholds_, const type &type_);
  void set_difference(const float difference, const type &type_);
  bool detect_change(const float current_co2, const float current_sprout_value,
                     const float current_voc);
  bool handle_change(const float current_co2, const float current_sprout_value,
                     const float current_voc);
  void check_lower_threshold(const type &type_);
  void check_all_lower_thresholds();
  void reset_ventilation();

 private:
  logging::logger m_logger;
  controller::control_array &m_control;
  bool m_output;
  bool m_active;
  float m_old_co2;
  float m_old_sprout_value;
  float m_old_voc;
  std::array<float, 3> m_differences;
  std::array<float, 3> m_mins;
  bool m_settings_changed;
  // 2 is the enum value of co2
  static constexpr int index_offset = 2;
};
}  // namespace controller
}  // namespace wolf

#endif  // WOLF_CONTROLLER_SVAS4_AIR_QUALITY_HPP
