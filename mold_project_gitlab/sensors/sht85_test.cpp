#include "sht85_test.hpp"

sht85_test::sht85_test(wolf::sht85_reader &reader)
    : m_logger("sht85_test"), m_reader(reader) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "start reading from sht85";
  m_reader.signal_read.connect(
      [this](const wolf::sht85_reader::read_buffer &data) {
        const wolf::types::data data_casted(data.cbegin(), data.cend());
        try {
          handle_read(data_casted);
        } catch (std::runtime_error &error) {
          LOGGING_LOG(m_logger, logging::severity::error)
              << "handle_read failed, error:" << error.what();
        }
      });
  try {
    m_reader.read_async(m_read_interval);
  } catch (std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not read from sht85, error: " << error.what();
  }
}

sht85_test::~sht85_test() { m_reader.close(); }

void sht85_test::handle_read(const wolf::types::data &data) {
  if (data.size() != 6) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_read with invalid data size:" << data.size();
    throw std::runtime_error("sht85_test::handle_read: invalid data size");
  }
  const wolf::sht85_parser::read_buffer data_casted = {
      {data[0], data[1], data[2], data[3], data[4], data[5]}};
  auto values = m_parser.parse(data_casted);
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "values.temperature: "
      << (values.temperature.has_value() ? std::to_string(*values.temperature)
                                         : "crc error")
      << ", values.humidity: "
      << (values.humidity.has_value() ? std::to_string(*values.humidity)
                                      : "crc error");
}
