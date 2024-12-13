#include "sht25_test.hpp"

using namespace logging;

sht25_test::sht25_test(boost::asio::io_service& service,
                       wolf::sht25_reader& reader)
    : m_logger("sht25_test"), m_timer_read(service), m_reader(reader) {
  LOGGING_LOG(m_logger, severity::normal) << "starting sht25_test";

  m_reader.read_async(4);
  m_reader.signal_read_humidity.connect(
      [&](const wolf::sht25_reader::read_buffer& buffer) {
        LOGGING_LOG(m_logger, severity::normal)
            << "humidity:"
            << m_parser.parse(wolf::sensor_sht25_channel::humidity, buffer);
      });
  m_reader.signal_read_temperature.connect(
      [&](const wolf::sht25_reader::read_buffer& buffer) {
        LOGGING_LOG(m_logger, severity::normal)
            << "temperature:"
            << m_parser.parse(wolf::sensor_sht25_channel::temperature, buffer);
      });
}

sht25_test::~sht25_test() {
  boost::system::error_code ignore_error;
  m_timer_read.cancel(ignore_error);
  m_reader.close();
}
