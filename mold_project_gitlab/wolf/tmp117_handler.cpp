#include "tmp117_handler.hpp"

#include "sensor_id_tmp117.hpp"

using namespace logging;

wolf::tmp117_handler::tmp117_handler(tmp117_reader &reader,
                                     const wolf::config &program_options)
    : m_logger("tmp117_handler"),
      m_reader(reader),
      m_program_options(program_options),
      m_filter_temperature(
          {program_options.tmp117_.temperature_filter_constant,
           program_options.tmp117_.temperature_filter_threshold,
           program_options.tmp117_.temperature_filter_threshold_with_time,
           seconds(program_options.tmp117_.filter_threshold_timeout)}) {}

void wolf::tmp117_handler::start_reading() {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from tmp117";
  m_reader.signal_read.connect(
      [this](const wolf::tmp117_reader::read_buffer &data) {
        const types::data data_casted(data.cbegin(), data.cend());
        handle_read(data_casted);
      });

  if (!m_reader.read_async(seconds(m_program_options.tmp117_.read_interval))) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read from tmp117, device not reachable";
  }
}

void wolf::tmp117_handler::resend_signals() {
  const auto last_temperature = m_filter_temperature.get_filtered_value();
  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (last_temperature.has_value()) {
    static const auto id =
        sensor_id_tmp117::create(sensor_tmp117_channel::temperature);
    values_to_signal.push_back({id, last_temperature.get(), now});
  }
  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

void wolf::tmp117_handler::handle_read(const wolf::types::data &data) {
  if (data.size() != 2) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_read with invalid data size:" << data.size();
    return;
  }

  const tmp117_parser::read_buffer data_casted = {{data[0], data[1]}};
  auto values = m_parser.parse(data_casted);

  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (values.temperature.has_value() &&
      filter_temperature(*values.temperature)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal temperature: " << std::dec << *values.temperature;
    static const auto id =
        sensor_id_tmp117::create(sensor_tmp117_channel::temperature);
    values_to_signal.push_back({id, *values.temperature, now});
  }

  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

bool wolf::tmp117_handler::filter_temperature(float &temperature) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_temperature";
  return m_filter_handler.filter_value(sensor_type::tmp117,
                                       m_filter_temperature, temperature);
}
