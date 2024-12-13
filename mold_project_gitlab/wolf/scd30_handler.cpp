#include "scd30_handler.hpp"
#include "sensor_id_scd30.hpp"

#include <boost/optional/optional_io.hpp>

using namespace logging;
wolf::scd30_handler::scd30_handler(boost::asio::io_service &service,
                                   scd30_reader &reader,
                                   const config &program_options)
    : m_logger("scd30_handler"),
      m_timer_read(service),
      m_timer_co2(service),
      m_reader(reader),
      m_program_options(program_options),
      m_filter_temperature(
          {program_options.scd30_.temperature_filter_constant,
           program_options.scd30_.temperature_filter_threshold,
           program_options.scd30_.temperature_filter_threshold_with_time,
           seconds(program_options.scd30_.filter_threshold_timeout)}),
      m_filter_humidity(
          {program_options.scd30_.humidity_filter_constant,
           program_options.scd30_.humidity_filter_threshold,
           program_options.scd30_.humidity_filter_threshold_with_time,
           seconds(program_options.scd30_.filter_threshold_timeout)}),
      m_filter_co2({program_options.scd30_.co2_filter_constant,
                    program_options.scd30_.co2_filter_threshold,
                    program_options.scd30_.co2_filter_threshold_with_time,
                    seconds(program_options.scd30_.filter_threshold_timeout)}),
      m_wait_timer_expired(false),
      m_asc_active(program_options.scd30_.use_asc) {
  start_wait_timer();
}

bool wolf::scd30_handler::start_reading(
    const std::optional<sensor_type> second_sensor) {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from scd30";
  if (!activate_asc()) return false;
  if (!trigger_continous_measurement()) return false;
  // second_sensor can only change after a restart because it is an onboard
  // sensor
  m_second_sensor = second_sensor;
  read_in_interval(start_interval);
  return true;
}

void wolf::scd30_handler::close() {
  boost::system::error_code ignore_error;
  m_timer_read.cancel(ignore_error);
}

void wolf::scd30_handler::resend_signals() {
  const auto last_temperature = m_filter_temperature.get_filtered_value();
  const auto last_humidity = m_filter_humidity.get_filtered_value();
  const auto last_co2 = m_filter_co2.get_filtered_value();
  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (last_temperature.has_value()) {
    static const auto id =
        sensor_id_scd30::create(sensor_scd30_channel::temperature);
    values_to_signal.push_back({id, last_temperature.get(), now});
  }
  if (last_humidity.has_value()) {
    static const auto id =
        sensor_id_scd30::create(sensor_scd30_channel::humidity);
    values_to_signal.push_back({id, last_humidity.get(), now});
  }
  if (last_co2.has_value()) {
    static const auto id = sensor_id_scd30::create(sensor_scd30_channel::co2);
    values_to_signal.push_back({id, last_co2.get(), now});
  }
  if (values_to_signal.empty()) return;
  signal_values(values_to_signal);
}

void wolf::scd30_handler::read_in_interval(const duration &interval) {
  m_timer_read.expires_from_now(interval);
  m_timer_read.async_wait([this](const boost::system::error_code &error) {
    if (error) return;
    read();
    read_in_interval(m_value_interval);
  });
}

void wolf::scd30_handler::read() {
  const auto ready_data = m_reader.get_data_ready_status();
  if (!ready_data.has_value()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "failed to read data ready status";
    return;
  }
  const bool ready = m_parser.parse_get_data_ready_status(*ready_data);
  LOGGING_LOG(m_logger, severity::verbose)
      << "ready: " << std::boolalpha << ready;
  if (!ready) return;

  const auto read_data = m_reader.get_data();
  if (!read_data.has_value()) {
    LOGGING_LOG(m_logger, severity::warning) << "failed to get data";
    return;
  }

  auto opt_values = m_parser.handle_data(*read_data);
  if (!opt_values.has_value()) {
    LOGGING_LOG(m_logger, severity::warning) << "failed to parse data";
    return;
  }
  auto values = *opt_values;

  const auto now = std::chrono::system_clock::now();
  values_type values_to_signal;
  if (use_temperature() && filter_temperature(values.temperature)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal temperature: " << std::dec << values.temperature;
    static const auto id =
        sensor_id_scd30::create(sensor_scd30_channel::temperature);
    values_to_signal.push_back({id, values.temperature, now});
  }

  if (use_humidity() && filter_humidity(values.humidity)) {
    LOGGING_LOG(m_logger, severity::normal)
        << "signal humidity: " << values.humidity;
    static const auto id =
        sensor_id_scd30::create(sensor_scd30_channel::humidity);
    values_to_signal.push_back({id, values.humidity, now});
  }

  if (use_co2() && filter_co2(values.co2)) {
    LOGGING_LOG(m_logger, severity::normal) << "signal CO2: " << values.co2;
    static const auto id = sensor_id_scd30::create(sensor_scd30_channel::co2);
    values_to_signal.push_back({id, values.co2, now});
  }

  if (values_to_signal.empty()) {
    return;
  }

  signal_values(values_to_signal);
}

bool wolf::scd30_handler::trigger_continous_measurement() {
  const auto interval_casted =
      std::chrono::duration_cast<seconds>(m_value_interval);
  const bool result = m_reader.trigger_continuous_measurement(
      static_cast<int>(interval_casted.count()));

  if (!result) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to trigger continuous measurement";
    return false;
  }
  return true;
}

bool wolf::scd30_handler::activate_asc() {
  const bool result = m_reader.set_asc_active(m_asc_active);
  if (!result) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to set asc active/inactive";
    return false;
  }
  return true;
}

bool wolf::scd30_handler::filter_temperature(float &temperature) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_temperature";
  return m_filter_handler.filter_value(sensor_type::scd30, m_filter_temperature,
                                       temperature);
}

bool wolf::scd30_handler::filter_humidity(float &humidity) {
  LOGGING_LOG(m_logger, severity::verbose) << "filter_humidity";
  return m_filter_handler.filter_value(sensor_type::scd30, m_filter_humidity,
                                       humidity);
}

bool wolf::scd30_handler::filter_co2(float &co2) {
  if (!m_wait_timer_expired) return false;
  LOGGING_LOG(m_logger, severity::verbose) << "filter_co2";
  return m_filter_handler.filter_value(sensor_type::scd30, m_filter_co2, co2);
}

void wolf::scd30_handler::start_wait_timer() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "wait with co2 reading for " << m_program_options.scd30_.wait_time_co2
      << " seconds";
  m_timer_co2.expires_from_now(co2_wait_time);
  m_timer_co2.async_wait([this](const boost::system::error_code &error) {
    handle_wait_timer_expired(error);
  });
}

void wolf::scd30_handler::handle_wait_timer_expired(
    const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "wait timer had an unexpected error, message:" << error.message();
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose) << "wait timer successfully expired";
  m_wait_timer_expired = true;
}

bool wolf::scd30_handler::use_temperature() const {
  if (!m_second_sensor.has_value()) return true;
  if (*m_second_sensor == sensor_type::sht25 ||
      *m_second_sensor == sensor_type::shtc1)
    return false;
  return true;
}

// although temperature and humidity check the same sensors they are split
// because they may differ in the future
bool wolf::scd30_handler::use_humidity() const {
  if (!m_second_sensor.has_value()) return true;
  if (*m_second_sensor == sensor_type::sht25 ||
      *m_second_sensor == sensor_type::shtc1)
    return false;
  return true;
}

bool wolf::scd30_handler::use_co2() const {
  if (!m_second_sensor.has_value()) return true;
  if (*m_second_sensor == sensor_type::ee895) return false;
  return true;
}
