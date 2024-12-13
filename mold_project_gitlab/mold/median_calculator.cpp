#include "median_calculator.hpp"

using namespace logging;

mold::median_calculator::median_calculator(const mold::config &config)
    : m_logger("median_calculator"),
      m_enabled{config.median_.use_median},
      m_median_time{std::chrono::minutes(
          config.median_.evaluation_humidity_median_time)} {}

float mold::median_calculator::calculate_median(
    const float value, const mold::median_calculator::timepoint &timepoint_) {
  if (!m_enabled) return value;
  LOGGING_LOG(m_logger, severity::verbose)
      << "calculate median for timestamp: "
      << timepoint_.time_since_epoch().count();
  m_values.push_back({value, timepoint_});
  if (m_values.size() == 1) return value;
  delete_old_values(timepoint_);
  sort_values();
  auto median = get_median_value(value);
  LOGGING_LOG(m_logger, severity::verbose) << "calculated median: " << median;
  return median;
}

float mold::median_calculator::get_median_value(const float value) {
  auto size = m_values.size();
  if (size == 0) return value;
  if (size == 1) return m_values.front().value;
  if (size % 2 == 0)
    return (m_values[size / 2 - 1].value + m_values[size / 2].value) / 2;
  return m_values[size / 2].value;
}

void mold::median_calculator::delete_old_values(const timepoint &now) {
  auto median_timepoint = now - m_median_time;
  m_values.erase(std::remove_if(m_values.begin(), m_values.end(),
                                [median_timepoint](value &value_) {
                                  return value_.timestamp < median_timepoint;
                                }),
                 m_values.end());
}

void mold::median_calculator::sort_values() {
  std::sort(m_values.begin(), m_values.end(),
            [](const value &value_1, const value &value_2) {
              return value_1.value < value_2.value;
            });
}
