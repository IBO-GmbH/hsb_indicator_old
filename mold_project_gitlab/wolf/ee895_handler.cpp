#include "ee895_handler.hpp"
#include "sensor_id_ee895.hpp"

using namespace logging;

wolf::ee895_handler::ee895_handler(wolf::ee895_reader &reader,
                                   const wolf::config &program_options)
    : m_logger("ee895_handler"),
      m_reader(reader),
      m_program_options(program_options),
      m_filter_co2({program_options.ee895_.co2_filter_constant,
                    program_options.ee895_.co2_filter_threshold,
                    program_options.ee895_.co2_filter_threshold_with_time,
                    seconds(program_options.ee895_.filter_threshold_timeout)}),
      m_filter_temperature(
          {program_options.ee895_.temperature_filter_constant,
           program_options.ee895_.temperature_filter_threshold,
           program_options.ee895_.temperature_filter_threshold_with_time,
           seconds(program_options.ee895_.filter_threshold_timeout)}),
      m_filter_air_pressure(
          {program_options.ee895_.air_pressure_filter_constant,
           program_options.ee895_.air_pressure_filter_threshold,
           program_options.ee895_.air_pressure_filter_threshold_with_time,
           seconds(program_options.ee895_.filter_threshold_timeout)}) {}

void wolf::ee895_handler::start_reading(
    const std::optional<sensor_type> second_sensor) {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from ee895";
  m_second_sensor = second_sensor;
  m_reader.signal_read.connect(
      [this](const wolf::ee895_reader::read_buffer &data) {
        handle_read(data);
      });

  if (!m_reader.read_async(seconds(m_program_options.ee895_.read_interval))) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read from ee895, device not reachable";
  }
}

void wolf::ee895_handler::resend_signals() {
  const auto last_co2 = m_filter_co2.get_filtered_value();
  const auto last_temperature = m_filter_temperature.get_filtered_value();
  const auto last_air_pressure = m_filter_air_pressure.get_filtered_value();
  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (last_co2.has_value()) {
    static const auto id = sensor_id_ee895::create(sensor_ee895_channel::co2);
    values_to_signal.push_back({id, last_co2.get(), now});
  }
  if (last_temperature.has_value()) {
    static const auto id =
        sensor_id_ee895::create(sensor_ee895_channel::temperature);
    values_to_signal.push_back({id, last_temperature.get(), now});
  }
  if (last_air_pressure.has_value()) {
    static const auto id =
        sensor_id_ee895::create(sensor_ee895_channel::air_pressure);
    values_to_signal.push_back({id, last_air_pressure.get(), now});
  }
  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

void wolf::ee895_handler::handle_read(const ee895_reader::read_buffer &data) {
  auto values = m_parser.parse(data);

  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;

  if (filter_co2(values.co2)) {
    LOGGING_LOG(m_logger, severity::normal) << "signal CO2: " << values.co2;
    static const auto id = sensor_id_ee895::create(sensor_ee895_channel::co2);
    values_to_signal.push_back({id, values.co2, now});
  }

  if (use_temperature() && filter_temperature(values.temperature)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal temperature: " << std::dec << values.temperature;
    static const auto id =
        sensor_id_ee895::create(sensor_ee895_channel::temperature);
    values_to_signal.push_back({id, values.temperature, now});
  }

  if (filter_air_pressure(values.air_pressure)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal air_pressure: " << values.air_pressure;
    static const auto id =
        sensor_id_ee895::create(sensor_ee895_channel::air_pressure);
    values_to_signal.push_back({id, values.air_pressure, now});
  }

  if (values_to_signal.empty()) {
    return;
  }

  signal_values(values_to_signal);
}

bool wolf::ee895_handler::filter_co2(float &co2) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_co2";
  return m_filter_handler.filter_value(sensor_type::ee895, m_filter_co2, co2);
}

bool wolf::ee895_handler::filter_temperature(float &temperature) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_temperature";
  return m_filter_handler.filter_value(sensor_type::ee895, m_filter_temperature,
                                       temperature);
}

bool wolf::ee895_handler::filter_air_pressure(float &air_pressure) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_air_pressure";
  return m_filter_handler.filter_value(sensor_type::ee895,
                                       m_filter_air_pressure, air_pressure);
}

bool wolf::ee895_handler::use_temperature() const {
  if (!m_second_sensor.has_value()) return true;
  if (*m_second_sensor == sensor_type::sht25 ||
      *m_second_sensor == sensor_type::shtc1 ||
      *m_second_sensor == sensor_type::scd30)
    return false;
  return true;
}
