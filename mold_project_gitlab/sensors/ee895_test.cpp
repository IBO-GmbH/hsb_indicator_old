#include "ee895_test.hpp"

using namespace logging;

ee895_test::ee895_test(wolf::ee895_reader &reader)
    : m_logger("ee895_test"), m_reader(reader) {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from ee895";
  m_reader.signal_read.connect(
      [this](const wolf::ee895_reader::read_buffer &data) {
        try {
          handle_read(data);
        } catch (std::runtime_error &error) {
          LOGGING_LOG(m_logger, severity::error)
              << "handle_read failed, error:" << error.what();
        }
      });

  try {
    if (!m_reader.read_async(m_read_interval)) {
      LOGGING_LOG(m_logger, severity::warning)
          << "could not read from ee895, device not reachable";
    }
  } catch (std::runtime_error &error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read from shtc1, error: " << error.what();
  }
}

ee895_test::~ee895_test() { m_reader.close(); }

void ee895_test::handle_read(const wolf::ee895_reader::read_buffer &data) {
  auto values = m_parser.parse(data);

  LOGGING_LOG(m_logger, severity::normal)
      << "values.co2: " << values.co2
      << ", values.temperature: " << values.temperature
      << ", values.air_pressure: " << values.air_pressure;
}
