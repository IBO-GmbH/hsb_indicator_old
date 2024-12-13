#include "shtc1_test.hpp"

using namespace logging;

shtc1_test::shtc1_test(wolf::shtc1_reader &reader)
    : m_logger("shtc1_test"), m_reader(reader) {
  LOGGING_LOG(m_logger, severity::verbose) << "start reading from shtc1";
  m_reader.signal_read.connect(
      [this](const wolf::shtc1_reader::read_buffer &data) {
        const wolf::types::data data_casted(data.cbegin(), data.cend());
        try {
          handle_read(data_casted);
        } catch (std::runtime_error &error) {
          LOGGING_LOG(m_logger, severity::error)
              << "handle_read failed, error:" << error.what();
        }
      });
  try {
    m_reader.read_async(m_read_interval);
  } catch (std::runtime_error &error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not read from shtc1, error: " << error.what();
  }
}

shtc1_test::~shtc1_test() { m_reader.close(); }

void shtc1_test::handle_read(const wolf::types::data &data) {
  if (data.size() != 6) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_read with invalid data size:" << data.size();
    throw std::runtime_error("shtc1_test::handle_read: invalid data size");
  }
  const wolf::shtc1_parser::read_buffer data_casted = {
      {data[0], data[1], data[2], data[3], data[4], data[5]}};
  auto values = m_parser.parse(data_casted);
  LOGGING_LOG(m_logger, severity::normal)
      << "values.temperature: "
      << (values.temperature.has_value() ? std::to_string(*values.temperature)
                                         : "crc error")
      << ", values.humidity: "
      << (values.humidity.has_value() ? std::to_string(*values.humidity)
                                      : "crc error");
}
