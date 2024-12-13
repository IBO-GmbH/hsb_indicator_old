#include "svas2_dehumidifier.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace logging;

wolf::controller::svas2_dehumidifier::svas2_dehumidifier(
    std::shared_ptr<controller_ventilation> controller_ventilation_)
    : m_logger("svas2_dehumidifier"),
      m_ventilation(controller_ventilation_),
      m_border(0.f),
      m_outputs({false, false}),
      m_active(false),
      m_old_relative_humidity(0.f),
      m_old_absolute_humidity(0.f),
      m_settings_changed(false) {
  m_ventilation->signal_thresholds_changed.connect([this] {
    reset_ventilation();
    m_settings_changed = true;
  });
}

void wolf::controller::svas2_dehumidifier::set_humidity_border(
    const float humidity_border) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set humidity_border to: " << humidity_border;
  if (wolf::math::compare_floats(m_border, humidity_border)) return;
  m_border = humidity_border;
  reset_dehumidifier();
}

void wolf::controller::svas2_dehumidifier::set_humidity_difference(
    const float difference_humidity) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set humidity_difference to: " << difference_humidity;
  if (wolf::math::compare_floats(m_difference, difference_humidity)) return;
  m_difference = difference_humidity;
  reset_dehumidifier();
}

float wolf::controller::svas2_dehumidifier::get_humidity_border() {
  return m_border;
}

void wolf::controller::svas2_dehumidifier::control(
    const float current_absolute_humidity,
    const float current_relative_humidity,
    const bool outdoor_temperature_right) {
  if (!control_loop_is_active()) {
    set_output_ventilation(false);
    set_output_dehumidifier(false);
    return;
  }

  if (!handle_change(current_absolute_humidity, current_relative_humidity))
    return;

  control_dehumidifier(current_absolute_humidity, current_relative_humidity);
  if (!outdoor_temperature_right) {
    set_output_ventilation(false);
    return;
  }
  if (!m_outputs.dehumidifier)
    control_ventilation(current_absolute_humidity, current_relative_humidity);
}

wolf::controller::svas2_dehumidifier::outputs
wolf::controller::svas2_dehumidifier::outputs_are_set() {
  return m_outputs;
}

void wolf::controller::svas2_dehumidifier::set_control_loop_active(
    const bool active_state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control loop svas2_dehumidifier set active: " << active_state;
  if (!active_state) {
    set_output_ventilation(false);
    set_output_dehumidifier(false);
  }
  m_active = active_state;
  m_settings_changed |= active_state;
}

bool wolf::controller::svas2_dehumidifier::control_loop_is_active() const {
  return m_active;
}

void wolf::controller::svas2_dehumidifier::reset_ventilation() {
  set_output_ventilation(false);
  m_settings_changed = true;
}

void wolf::controller::svas2_dehumidifier::reset_dehumidifier() {
  set_output_dehumidifier(false);
  m_settings_changed = true;
}

void wolf::controller::svas2_dehumidifier::control_ventilation(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  set_output_dehumidifier(false);
  m_ventilation->control(current_absolute_humidity, current_relative_humidity);
  set_output_ventilation(m_ventilation->output_is_set());
}

void wolf::controller::svas2_dehumidifier::control_dehumidifier(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control dehumidifier, values: " << current_relative_humidity;
  set_output_ventilation(false);
  float check_humidity = m_border;
  if (m_outputs.dehumidifier) check_humidity = m_border - m_difference;
  if (current_relative_humidity >= check_humidity) {
    set_output_dehumidifier(true);
    return;
  }
  control_ventilation(current_absolute_humidity, current_relative_humidity);
}

void wolf::controller::svas2_dehumidifier::set_output_ventilation(
    const bool state) {
  if (m_outputs.ventilation == state) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "set output ventilation to: " << state;
  m_outputs.ventilation = state;
}

void wolf::controller::svas2_dehumidifier::set_output_dehumidifier(
    const bool state) {
  if (m_outputs.dehumidifier == state) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "set output dehumidifier to: " << state;
  m_outputs.dehumidifier = state;
}

bool wolf::controller::svas2_dehumidifier::detect_change(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  return !wolf::math::compare_floats(m_old_relative_humidity,
                                     current_relative_humidity) ||
         !wolf::math::compare_floats(m_old_absolute_humidity,
                                     current_absolute_humidity) ||
         m_settings_changed;
}

bool wolf::controller::svas2_dehumidifier::handle_change(
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
