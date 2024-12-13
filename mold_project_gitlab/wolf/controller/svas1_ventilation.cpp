#include "svas1_ventilation.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace logging;

wolf::controller::svas1_ventilation::svas1_ventilation(
    std::shared_ptr<controller_ventilation> controller_ventilation_)
    : m_logger("svas1_ventilation"),
      m_ventilation(controller_ventilation_),
      m_active(false),
      m_output(false),
      m_old_relative_humidity(0.f),
      m_old_absolute_humidity(0.f),
      m_settings_changed(false) {
  m_ventilation->signal_thresholds_changed.connect([this] {
    reset_ventilation();
    m_settings_changed = true;
  });
}

void wolf::controller::svas1_ventilation::control(
    const float current_absolute_humidity,
    const float current_relative_humidity,
    const bool outdoor_temperature_right) {
  if (!control_loop_is_active() || !outdoor_temperature_right) {
    set_output(false);
    return;
  }

  if (!handle_change(current_absolute_humidity, current_relative_humidity))
    return;

  m_ventilation->control(current_absolute_humidity, current_relative_humidity);
  set_output(m_ventilation->output_is_set());
}

bool wolf::controller::svas1_ventilation::output_is_set() { return m_output; }

void wolf::controller::svas1_ventilation::set_control_loop_active(
    const bool active_state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control loop svas1_ventilation set active: " << active_state;
  if (!active_state) set_output(false);
  m_active = active_state;
  m_settings_changed |= active_state;
}

bool wolf::controller::svas1_ventilation::control_loop_is_active() const {
  return m_active;
}

void wolf::controller::svas1_ventilation::reset_ventilation() {
  set_output(false);
  m_settings_changed = true;
}

void wolf::controller::svas1_ventilation::set_output(const bool state) {
  if (m_output == state) return;
  LOGGING_LOG(m_logger, severity::verbose) << "set output to: " << state;
  m_output = state;
}

bool wolf::controller::svas1_ventilation::detect_change(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  return !wolf::math::compare_floats(m_old_relative_humidity,
                                     current_relative_humidity) ||
         !wolf::math::compare_floats(m_old_absolute_humidity,
                                     current_absolute_humidity) ||
         m_settings_changed;
}

bool wolf::controller::svas1_ventilation::handle_change(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  bool changed =
      detect_change(current_absolute_humidity, current_relative_humidity);
  if (!changed) return false;
  m_old_relative_humidity = current_relative_humidity;
  m_old_absolute_humidity = current_absolute_humidity;
  m_settings_changed = false;
  return true;
}
