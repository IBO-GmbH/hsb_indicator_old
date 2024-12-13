#include "scd30_test.hpp"
#include <thread>

using namespace logging;

scd30_test::scd30_test(boost::asio::io_service& service,
                       wolf::scd30_reader& reader)
    : m_logger("scd30_test"), m_timer_read(service), m_reader(reader) {
  LOGGING_LOG(m_logger, severity::normal) << "starting scd30_test";

  m_reader.set_asc_active(true);
  m_reader.trigger_continuous_measurement(2);
  read_in_a_second();
}

scd30_test::~scd30_test() {
  boost::system::error_code ignore_error;
  m_timer_read.cancel(ignore_error);
  m_reader.close();
}

void scd30_test::read_in_a_second() {
  m_timer_read.expires_from_now(std::chrono::seconds(1));
  m_timer_read.async_wait([this](const boost::system::error_code& error) {
    if (error) return;
    try {
      read();
    } catch (const std::runtime_error& error) {
      LOGGING_LOG(m_logger, severity::warning)
          << "an error occured when trying to read from sensor, what:"
          << error.what();
    }
    read_in_a_second();
  });
}

void scd30_test::read() {
  const auto ready_data = m_reader.get_data_ready_status();
  if (!ready_data.has_value()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "failed to get data ready status";
    return;
  }

  const bool ready = m_parser.parse_get_data_ready_status(*ready_data);
  LOGGING_LOG(m_logger, severity::normal)
      << "ready:" << std::boolalpha << ready;
  if (!ready) return;
  const auto read_data = m_reader.get_data();
  if (!read_data.has_value()) {
    LOGGING_LOG(m_logger, severity::warning) << "failed to get data";
    return;
  }

  const auto result = m_parser.handle_data(*read_data);
  if (!result.has_value()) {
    LOGGING_LOG(m_logger, severity::warning) << "failed to parse data";
    return;
  }

  LOGGING_LOG(m_logger, severity::normal)
      << "result.co2:" << std::dec << result->co2
      << ", result.humidity:" << result->humidity
      << ", result.temperature:" << result->temperature;
}
