#include "sgpc3_test.hpp"
#include <boost/optional/optional_io.hpp>

using namespace logging;

sgpc3_test::sgpc3_test(boost::asio::io_service& service,
                       wolf::sgpc3_reader& reader)
    : m_logger("sgpc3_test"), m_timer_read(service), m_reader(reader) {
  LOGGING_LOG(m_logger, severity::normal) << "starting sgpc3_test";

  m_reader.signal_read_data.connect(
      [this](const wolf::sgpc3_reader::read_buffer& buffer) {
        const auto result = m_parser.handle_data(buffer);
        LOGGING_LOG(m_logger, severity::normal) << "tvoc:" << result;
        wait_and_read_again();
      });
  m_next_read = std::chrono::steady_clock::now();
  wait_and_read_again();
}

sgpc3_test::~sgpc3_test() {
  boost::system::error_code ignore_error;
  m_timer_read.cancel(ignore_error);
  m_reader.close();
}

void sgpc3_test::wait_and_read_again() {
  m_next_read += std::chrono::seconds(2);
  m_timer_read.expires_at(m_next_read);
  m_timer_read.async_wait([&](const boost::system::error_code& error) {
    if (error) return;
    m_reader.read_data_async();
  });
}
