#include "svas3_heating.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace logging;

wolf::controller::svas3_heating::svas3_heating(
    std::shared_ptr<controller_ventilation> controller_ventilation_)
    : m_logger("svas3_heating"),
      m_ventilation(controller_ventilation_),
      m_outputs({false, false}),
      m_active(false),
      m_old_relative_humidity(0.f),
      m_old_absolute_humidity(0.f),
      m_old_temperature(0.f),
      m_settings_changed(false) {
  m_ventilation->signal_thresholds_changed.connect([this] {
    reset_ventilation();
    m_settings_changed = true;
  });
}

void wolf::controller::svas3_heating::set_limits(
    const limits::temperature& limits) {
  if (m_limits == limits) return;
  m_limits = limits;
  reset_heating();
  m_settings_changed = true;
}

wolf::controller::limits::temperature
wolf::controller::svas3_heating::get_limits() {
  return m_limits;
}

void wolf::controller::svas3_heating::control(
    const float current_absolute_humidity,
    const float current_relative_humidity, const float current_temperature,
    const bool outdoor_temperature_right) {
  if (!control_loop_is_active()) {
    set_output_ventilation(false);
    set_output_heating(false);
    return;
  }

  if (!handle_change(current_absolute_humidity, current_relative_humidity,
                     current_temperature))
    return;

  control_heating(current_temperature);
  if (!outdoor_temperature_right) {
    set_output_ventilation(false);
    return;
  }
  if (!m_outputs.heating)
    control_ventilation(current_absolute_humidity, current_relative_humidity);
}

wolf::controller::svas3_heating::outputs
wolf::controller::svas3_heating::outputs_are_set() {
  return m_outputs;
}

void wolf::controller::svas3_heating::set_control_loop_active(
    const bool active_state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control loop svas3_heating set active: " << active_state;
  if (!active_state) {
    set_output_ventilation(false);
    set_output_heating(false);
  }
  m_active = active_state;
  m_settings_changed |= active_state;
}

bool wolf::controller::svas3_heating::control_loop_is_active() const {
  return m_active;
}

void wolf::controller::svas3_heating::reset_ventilation() {
  set_output_ventilation(false);
  m_settings_changed = true;
}

void wolf::controller::svas3_heating::reset_heating() {
  set_output_heating(false);
  m_settings_changed = true;
}

void wolf::controller::svas3_heating::control_ventilation(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  set_output_heating(false);
  m_ventilation->control(current_absolute_humidity, current_relative_humidity);
  set_output_ventilation(m_ventilation->output_is_set());
}

bool wolf::controller::svas3_heating::check_temperature(
    const float current_temperature) {
  if (current_temperature >= m_limits.reference) return false;
  if (current_temperature < m_limits.min) return true;
  if (current_temperature >= m_limits.min) {
    if ((current_temperature < m_limits.reference) && m_outputs.heating)
      return true;
    return false;
  }
  return false;
}

void wolf::controller::svas3_heating::control_heating(
    const float current_temperature) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control heating, values: " << current_temperature << ", "
      << m_limits.min;
  set_output_ventilation(false);
  if (check_temperature(current_temperature)) {
    set_output_heating(true);
    return;
  }
  set_output_heating(false);
}

void wolf::controller::svas3_heating::set_output_ventilation(const bool state) {
  if (m_outputs.ventilation == state) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "set output ventilation to: " << state;
  m_outputs.ventilation = state;
}

void wolf::controller::svas3_heating::set_output_heating(const bool state) {
  if (m_outputs.heating == state) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "set output heating to: " << state;
  m_outputs.heating = state;
}

bool wolf::controller::svas3_heating::detect_change(
    const float current_absolute_humidity,
    const float current_relative_humidity, const float current_temperature) {
  return !wolf::math::compare_floats(m_old_relative_humidity,
                                     current_relative_humidity) ||
         !wolf::math::compare_floats(m_old_absolute_humidity,
                                     current_absolute_humidity) ||
         !wolf::math::compare_floats(m_old_temperature, current_temperature) ||
         m_settings_changed;
}

bool wolf::controller::svas3_heating::handle_change(
    const float current_absolute_humidity,
    const float current_relative_humidity, const float current_temperature) {
  bool changed = detect_change(current_absolute_humidity,
                               current_relative_humidity, current_temperature);
  if (!changed) return false;
  m_old_relative_humidity = current_relative_humidity;
  m_old_absolute_humidity = current_absolute_humidity;
  m_old_temperature = current_temperature;
  m_settings_changed = false;
  return true;
}
