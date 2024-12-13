#include "svas4_air_quality.hpp"

#include "wolf/math_compare_decimals.hpp"

using namespace logging;

wolf::controller::svas4_air_quality::svas4_air_quality(
    controller::control_array &control)
    : m_logger("svas4_air_quality"),
      m_control(control),
      m_output(false),
      m_active(false),
      m_old_co2(0.f),
      m_old_sprout_value(0.f),
      m_old_voc(0.f),
      m_settings_changed(false) {}

void wolf::controller::svas4_air_quality::control(
    const float current_co2, const float current_sprout_value,
    const float current_voc, const bool outdoor_temperature_right) {
  if (!control_loop_is_active() || !outdoor_temperature_right) {
    set_output(false);
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << "control air quality, values: " << current_co2 << ", "
      << current_sprout_value << ", " << current_voc;

  if (!handle_change(current_co2, current_sprout_value, current_voc)) return;

  check_all_lower_thresholds();
  if (check_control(current_co2, current_sprout_value, current_voc)) {
    set_output(true);
    return;
  }
  set_output(false);
}

void wolf::controller::svas4_air_quality::set_co2_thresholds(
    const wolf::controller::thresholds &co2) {
  set_thresholds(co2, type::co2);
}

void wolf::controller::svas4_air_quality::set_co2_difference(
    const float difference) {
  set_difference(difference, type::co2);
}

void wolf::controller::svas4_air_quality::set_sprout_value_thresholds(
    const wolf::controller::thresholds &sprout_value) {
  set_thresholds(sprout_value, type::sprout_value);
}

void wolf::controller::svas4_air_quality::set_sprout_value_difference(
    const float difference) {
  set_difference(difference, type::sprout_value);
}

void wolf::controller::svas4_air_quality::set_voc_thresholds(
    const wolf::controller::thresholds &voc) {
  set_thresholds(voc, type::voc);
}

void wolf::controller::svas4_air_quality::set_voc_difference(
    const float difference) {
  set_difference(difference, type::voc);
}

bool wolf::controller::svas4_air_quality::output_is_set() { return m_output; }

void wolf::controller::svas4_air_quality::set_control_loop_active(
    const bool active_state) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "control loop svas4_air_quality set active: " << active_state;
  if (!active_state) set_output(false);
  m_active = active_state;
  m_settings_changed |= active_state;
}

bool wolf::controller::svas4_air_quality::control_loop_is_active() const {
  return m_active;
}

bool wolf::controller::svas4_air_quality::control(
    const float current_value, const wolf::controller::type &type_) const {
  auto &controller = get_on_of_control(type_);
  controller.control_output(current_value);
  return controller.output_is_set();
}

wolf::controller::on_off_control &
wolf::controller::svas4_air_quality::get_on_of_control(
    const wolf::controller::type &type_) const {
  auto type_as_uint = static_cast<unsigned int>(type_);
  auto &controller = m_control[type_as_uint];
  return controller;
}

bool wolf::controller::svas4_air_quality::check_control(
    const float current_co2, const float current_sprout_value,
    const float current_voc) const {
  auto co2_set = control(current_co2, type::co2);
  auto sprout_value_set = control(current_sprout_value, type::sprout_value);
  auto voc_set = control(current_voc, type::voc);
  return co2_set || sprout_value_set || voc_set;
}

void wolf::controller::svas4_air_quality::set_output(const bool state) {
  if (m_output == state) return;
  LOGGING_LOG(m_logger, severity::verbose) << "set output to: " << state;
  m_output = state;
}

void wolf::controller::svas4_air_quality::set_thresholds(
    const wolf::controller::thresholds &thresholds_,
    const wolf::controller::type &type_) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set thresholds for: " << type_ << " to: " << thresholds_;
  auto type_casted = static_cast<unsigned int>(type_);
  if (wolf::math::compare_floats(m_control[type_casted].get_thresholds().lower,
                                 thresholds_.lower))
    return;
  const auto index = type_casted - index_offset;
  m_mins[index] = thresholds_.lower;
  m_control[type_casted].set_thresholds(thresholds_);
  reset_ventilation();
}

void wolf::controller::svas4_air_quality::set_difference(
    const float difference, const wolf::controller::type &type_) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set difference for: " << type_ << " to: " << difference;
  auto type_casted = static_cast<unsigned int>(type_);
  const auto index = type_casted - index_offset;
  if (wolf::math::compare_floats(m_differences[index], difference)) return;
  m_differences[index] = difference;
  reset_ventilation();
}

bool wolf::controller::svas4_air_quality::detect_change(
    const float current_co2, const float current_sprout_value,
    const float current_voc) {
  return !wolf::math::compare_floats(m_old_co2, current_co2) ||
         !wolf::math::compare_floats(m_old_sprout_value,
                                     current_sprout_value) ||
         !wolf::math::compare_floats(m_old_voc, current_voc) ||
         m_settings_changed;
}

bool wolf::controller::svas4_air_quality::handle_change(
    const float current_co2, const float current_sprout_value,
    const float current_voc) {
  bool changed = detect_change(current_co2, current_sprout_value, current_voc);
  if (!changed) return false;
  m_old_co2 = current_co2;
  m_old_sprout_value = current_sprout_value;
  m_old_voc = current_voc;
  m_settings_changed = false;
  return true;
}

void wolf::controller::svas4_air_quality::check_lower_threshold(
    const wolf::controller::type &type_) {
  auto &controller = get_on_of_control(type_);
  const auto thresholds_ = controller.get_thresholds();
  const auto max = thresholds_.upper;
  const auto min = thresholds_.lower;
  const auto index = static_cast<unsigned int>(type_) - index_offset;
  if (m_output) {
    auto new_lower_threshold = m_mins[index] - m_differences[index];
    if (!wolf::math::compare_floats(new_lower_threshold, min)) {
      LOGGING_LOG(m_logger, severity::verbose)
          << "new lower threshold: " << new_lower_threshold
          << " for type: " << type_;
      controller.set_thresholds({max, new_lower_threshold});
    }
    return;
  }
  if (!wolf::math::compare_floats(m_mins[index], min))
    controller.set_thresholds({max, m_mins[index]});
}

void wolf::controller::svas4_air_quality::check_all_lower_thresholds() {
  check_lower_threshold(type::co2);
  check_lower_threshold(type::sprout_value);
  check_lower_threshold(type::voc);
}

void wolf::controller::svas4_air_quality::reset_ventilation() {
  set_output(false);
  m_settings_changed = true;
}
