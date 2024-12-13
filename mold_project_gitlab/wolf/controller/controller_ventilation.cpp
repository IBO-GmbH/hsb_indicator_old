#include "controller_ventilation.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace logging;

wolf::controller::controller_ventilation::controller_ventilation(
    controller::control_array &control)
    : m_logger("controller_ventilation"), m_control(control), m_output(false) {}

void wolf::controller::controller_ventilation::set_difference_humidity(
    const float difference_humidity) {
  if (wolf::math::compare_floats(m_difference_humidity, difference_humidity))
    return;
  m_difference_humidity = difference_humidity;
  signal_thresholds_changed();
}

float wolf::controller::controller_ventilation::get_difference_humidity() {
  return m_difference_humidity;
}

void wolf::controller::controller_ventilation::save_lower_threshold(
    const float lower) {
  m_min = lower;
}

bool wolf::controller::controller_ventilation::compare_thresholds(
    const wolf::controller::thresholds &old,
    const wolf::controller::thresholds &new_) {
  return (wolf::math::compare_floats(old.upper, new_.upper) &&
          wolf::math::compare_floats(old.lower, new_.lower));
}

void wolf::controller::controller_ventilation::set_thresholds(
    const wolf::controller::thresholds &thresholds_,
    const wolf::controller::type &type_) {
  auto type_casted = static_cast<unsigned int>(type_);
  if (compare_thresholds(m_control[type_casted].get_thresholds(), thresholds_))
    return;
  m_control[type_casted].set_thresholds(thresholds_);
  reset_ventilation();
  signal_thresholds_changed();
}

void wolf::controller::controller_ventilation::control(
    const float current_absolute_humidity,
    const float current_relative_humidity) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control ventilation, values: " << current_absolute_humidity << ", "
      << current_relative_humidity;
  check_lower_threshold();
  auto absolute_humidity_set =
      control(current_absolute_humidity, type::absolute_humidity);
  auto relative_humidity_set =
      control(current_relative_humidity, type::relative_humidity);
  if (absolute_humidity_set && relative_humidity_set) {
    set_output(true);
    return;
  }
  set_output(false);
}

bool wolf::controller::controller_ventilation::output_is_set() {
  return m_output;
}

void wolf::controller::controller_ventilation::reset_ventilation() {
  set_output(false);
}

void wolf::controller::controller_ventilation::set_absolute_humidity_thresholds(
    const wolf::controller::thresholds &absolute_humidity) {
  set_thresholds(absolute_humidity, type::absolute_humidity);
}

void wolf::controller::controller_ventilation::set_relative_humidity_thresholds(
    const wolf::controller::thresholds &relative_humidity) {
  save_lower_threshold(relative_humidity.lower);
  set_thresholds(relative_humidity, type::relative_humidity);
}

bool wolf::controller::controller_ventilation::control(
    const float current_value, const wolf::controller::type &type_) {
  auto &controller = get_on_of_control(type_);
  controller.control_output(current_value);
  return controller.output_is_set();
}

void wolf::controller::controller_ventilation::check_lower_threshold() {
  auto &controller = get_on_of_control(type::relative_humidity);
  const auto thresholds_ = controller.get_thresholds();
  const auto max = thresholds_.upper;
  const auto min = thresholds_.lower;
  if (m_output) {
    auto new_lower_threshold = m_min - m_difference_humidity;
    if (!wolf::math::compare_floats(new_lower_threshold, min)) {
      LOGGING_LOG(m_logger, severity::verbose)
          << "new lower threshold: " << new_lower_threshold;
      controller.set_thresholds({max, new_lower_threshold});
    }
    return;
  }
  if (!wolf::math::compare_floats(m_min, min))
    controller.set_thresholds({max, m_min});
}

wolf::controller::on_off_control &
wolf::controller::controller_ventilation::get_on_of_control(
    const wolf::controller::type &type_) {
  auto type_as_uint = static_cast<unsigned int>(type_);
  auto &controller = m_control[type_as_uint];
  return controller;
}

void wolf::controller::controller_ventilation::set_output(
    const bool output_state) {
  m_output = output_state;
}
