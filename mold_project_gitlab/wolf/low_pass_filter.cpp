#include "low_pass_filter.hpp"

using namespace logging;
using namespace wolf;

low_pass_filter::low_pass_filter(
    const low_pass_filter_data &low_pass_filter_data)
    : m_logger("low_pass_filter"),
      m_filter_constant{low_pass_filter_data.filter_constant},
      m_threshold{low_pass_filter_data.threshold},
      m_threshold_with_time{low_pass_filter_data.threshold_with_time},
      m_max_age(low_pass_filter_data.max_age) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "create with filter constant:" << m_filter_constant
      << " threshold:" << m_threshold
      << " threshold with time:" << m_threshold_with_time
      << " max age:" << m_max_age.count();
}

bool low_pass_filter::handle_value(const float &value) {
  return handle_value(value, std::chrono::steady_clock::now());
}

bool low_pass_filter::handle_value(
    const float &value, const std::chrono::steady_clock::time_point &now) {
  if (!m_value) {
    LOGGING_LOG(m_logger, severity::verbose) << "setting initial value";
    trigger(value);
    return true;
  }

  value_container &values = m_value.get();

  const auto time_diff = now - values.time_last_triggered;
  if (time_diff > m_max_age && m_max_age.count() > 0) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "time_diff > m_max_age && m_max_age.count() > 0"
           ", time_diff:"
        << std::chrono::duration_cast<std::chrono::seconds>(time_diff).count()
        << "s, m_max_age:" << m_max_age.count() << "s";
    trigger(value);
    return true;
  }
  const std::chrono::duration<float> diff_since_last_triggerd = time_diff;

  const auto lastFiltered = values.filtered;
  values.filtered += (value - values.filtered) * m_filter_constant;
  const float diff = std::abs(values.filtered - values.last_triggered);

  LOGGING_LOG(m_logger, severity::verbose)
      << "value:" << value << " last filtered:" << lastFiltered
      << " values.filtered:" << values.filtered
      << " m_filter_constant:" << m_filter_constant
      << " values.last_triggered:" << values.last_triggered << " diff:" << diff
      << " threshold:" << m_threshold;

  if (diff > m_threshold && m_threshold > 0.f) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "diff > m_threshold diff:" << diff;
    trigger(value);
    return true;
  }
  if (diff * diff_since_last_triggerd.count() > m_threshold_with_time &&
      m_threshold_with_time > 0.f) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "diff * diff_since_last_triggerd.count() > m_threshold_with_time"
        << " (diff * diff_since_last_triggerd.count()):"
        << (diff * diff_since_last_triggerd.count());
    trigger(value);
    return true;
  }
  return false;
}

boost::optional<float> low_pass_filter::get_filtered_value() const {
  if (m_value) {
    return m_value.get().filtered;
  }
  return {};
}

void low_pass_filter::trigger(const float &value) {
  LOGGING_LOG(m_logger, severity::verbose) << "trigger value:" << value;

  m_value = value_container{value, value, std::chrono::steady_clock::now()};
  signal_change(value);
}
