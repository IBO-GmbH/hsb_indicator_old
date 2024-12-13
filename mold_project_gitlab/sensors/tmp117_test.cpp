#include "tmp117_test.hpp"

tmp117_test::tmp117_test(wolf::tmp117_reader &reader)
    : m_logger("tmp117_test"), m_reader(reader) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "start reading from tmp117";
  m_reader.signal_read.connect(
      [this](const wolf::tmp117_reader::read_buffer &data) {
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
        << "could not read from tmp117, error: " << error.what();
  }
}

tmp117_test::~tmp117_test() { m_reader.close(); }

void tmp117_test::handle_read(const wolf::types::data &data) {
  if (data.size() != 2) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle_read with invalid data size:" << data.size();
    throw std::runtime_error("tmp117_test::handle_read: invalid data size");
  }
  const wolf::tmp117_parser::read_buffer data_casted = {{data[0], data[1]}};
  auto values = m_parser.parse(data_casted);
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "values.temperature: "
      << (values.temperature.has_value() ? std::to_string(*values.temperature)
                                         : "crc error");
}
