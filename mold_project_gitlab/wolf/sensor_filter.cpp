#include "sensor_filter.hpp"

#include <boost/optional/optional_io.hpp>

using namespace logging;

wolf::sensor_filter::sensor_filter() : m_logger("sensor_filter") {}

bool wolf::sensor_filter::filter_value(const wolf::sensor_type &type,
                                       wolf::low_pass_filter &filter,
                                       float &value) {
  bool emit_signals{false};
  const auto before = filter.get_filtered_value();
  emit_signals = filter.handle_value(value);
  const auto filtered_value = filter.get_filtered_value();
  LOGGING_LOG(m_logger, severity::verbose)
      << "filter_value() type: " << type << " value : " << value
      << " filtered value:" << filtered_value
      << " previous filtered value: " << before
      << " emit signals:" << emit_signals;
  if (!filtered_value) return false;
  value = filtered_value.get();
  return emit_signals;
}
