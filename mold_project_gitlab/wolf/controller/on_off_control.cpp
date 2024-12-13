#include "on_off_control.hpp"

#include <ostream>

std::ostream &wolf::controller::operator<<(std::ostream &out,
                                           const on_off_control &to_print) {
  out << to_print.m_on_off_values;
  return out;
}

using namespace wolf;
using namespace logging;
using namespace controller;

on_off_control::on_off_control() : m_logger("on_off_control") {}

void on_off_control::set_thresholds(const thresholds &thresholds) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set upper_threshold to: " << thresholds.upper
      << ", set lower_threshold to: " << thresholds.lower;
  m_on_off_values.threshold.upper = thresholds.upper;
  m_on_off_values.threshold.lower = thresholds.lower;
}

thresholds on_off_control::get_thresholds() const {
  return m_on_off_values.threshold;
}

void on_off_control::control_output(const float sensor_value) {
  if (!check_value(sensor_value)) {
    set_output(false);
    return;
  }
  set_output(true);
}

bool on_off_control::output_is_set() const { return m_on_off_values.output; }

bool on_off_control::check_value(const float sensor_value) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "compare sensor_value " << sensor_value << " to thresholds";
  if (sensor_value >= m_on_off_values.threshold.upper)
    return false;
  else if (sensor_value <= m_on_off_values.threshold.lower)
    return false;
  return true;
}

void on_off_control::set_output(bool output_state) {
  LOGGING_LOG(m_logger, severity::normal)
      << "set output to: " << output_state;
  m_on_off_values.output = output_state;
}
