#include "shtc1_handler.hpp"

#include "sensor_id_shtc1.hpp"

using namespace logging;

wolf::shtc1_handler::shtc1_handler(shtc1_reader &reader,
                                   const wolf::config &program_options)
    : m_logger("shtc1_handler"),
      m_reader(reader),
      m_program_options(program_options),
      m_filter_temperature(
          {program_options.shtc1_.temperature_filter_constant,
           program_options.shtc1_.temperature_filter_threshold,
           program_options.shtc1_.temperature_filter_threshold_with_time,
           seconds(program_options.shtc1_.filter_threshold_timeout)}),
      m_filter_humidity(
          {program_options.shtc1_.humidity_filter_constant,
           program_options.shtc1_.humidity_filter_threshold,
           program_options.shtc1_.humidity_filter_threshold_with_time,
           seconds(program_options.shtc1_.filter_threshold_timeout)}) {}

void wolf::shtc1_handler::start_reading() {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from shtc1";
  m_reader.signal_read.connect(
      [this](const wolf::shtc1_reader::read_buffer &data) {
        const types::data data_casted(data.cbegin(), data.cend());
        handle_read(data_casted);
      });

  if (!m_reader.read_async(seconds(m_program_options.shtc1_.read_interval))) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read from shtc1, device not reachable";
  }
}

void wolf::shtc1_handler::resend_signals() {
  const auto last_temperature = m_filter_temperature.get_filtered_value();
  const auto last_humidity = m_filter_humidity.get_filtered_value();
  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (last_temperature.has_value()) {
    static const auto id =
        sensor_id_shtc1::create(sensor_shtc1_channel::temperature);
    values_to_signal.push_back({id, last_temperature.get(), now});
  }
  if (last_humidity.has_value()) {
    static const auto id =
        sensor_id_shtc1::create(sensor_shtc1_channel::humidity);
    values_to_signal.push_back({id, last_humidity.get(), now});
  }
  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

void wolf::shtc1_handler::handle_read(const wolf::types::data &data) {
  if (data.size() != 6) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_read with invalid data size:" << data.size();
    return;
  }

  const shtc1_parser::read_buffer data_casted = {
      {data[0], data[1], data[2], data[3], data[4], data[5]}};
  auto values = m_parser.parse(data_casted);

  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (values.temperature.has_value() &&
      filter_temperature(*values.temperature)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal temperature: " << std::dec << *values.temperature;
    static const auto id =
        sensor_id_shtc1::create(sensor_shtc1_channel::temperature);
    values_to_signal.push_back({id, *values.temperature, now});
  }

  if (values.humidity.has_value() && filter_humidity(*values.humidity)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal humidity: " << *values.humidity;
    static const auto id =
        sensor_id_shtc1::create(sensor_shtc1_channel::humidity);
    values_to_signal.push_back({id, *values.humidity, now});
  }

  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

bool wolf::shtc1_handler::filter_temperature(float &temperature) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_temperature";
  return m_filter_handler.filter_value(sensor_type::shtc1, m_filter_temperature,
                                       temperature);
}

bool wolf::shtc1_handler::filter_humidity(float &humidity) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_humidity";
  return m_filter_handler.filter_value(sensor_type::shtc1, m_filter_humidity,
                                       humidity);
}
