#include "ee895_reader.hpp"
#include "i2c/open_close.hpp"

using namespace logging;

wolf::ee895_reader::ee895_reader(boost::asio::io_service &service)
    : m_logger("ee895_reader"),
      m_timer(service),
      m_read_interval{},
      m_reader_writer(m_logger, m_file) {}

bool wolf::ee895_reader::open(const std::string &device, const int slave_id) {
  LOGGING_LOG(m_logger, severity::normal)
      << "opening device:" << device << " using slave_id:0x" << std::hex
      << slave_id;

  if (auto file = i2c::open_close::open(device, slave_id); file.has_value()) {
    m_file = *file;
  } else {
    LOGGING_LOG(m_logger, severity::normal)
        << "opening device:" << device << " failed";
    return false;
  }
  return check_open();
}

void wolf::ee895_reader::close() {
  boost::system::error_code ignore_error;
  m_timer.cancel(ignore_error);
  i2c::open_close::close(m_file);
}

bool wolf::ee895_reader::read_async(wolf::ee895_reader::seconds read_interval) {
  if (read_interval < m_min_read_interval) {
    LOGGING_LOG(m_logger, severity::warning)
        << "read_interval below min interval, defaulting to "
        << m_min_read_interval.count() << " seconds";
    read_interval = m_min_read_interval;
  }

  m_read_interval = read_interval;
  return read();
}

bool wolf::ee895_reader::check_open() {
  std::uint8_t check_byte = 0;
  return send_command(check_byte);
}

bool wolf::ee895_reader::read() {
  if (!send_command(m_request_measurement)) {
    trigger_read();
    return false;
  }
  boost::system::error_code no_error;
  handle_read(no_error);
  return true;
}

void wolf::ee895_reader::handle_read(const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_read, error: " << error.message();
    return;
  }

  read_buffer buffer;
  if (m_reader_writer.read_into_buffer(buffer)) {
    signal_read(buffer);
  }

  trigger_read();
}

void wolf::ee895_reader::trigger_read() {
  m_timer.expires_from_now(m_read_interval);
  m_timer.async_wait([this](const boost::system::error_code &error) {
    if (error) {
      if (error.value() == boost::asio::error::operation_aborted)
        // timer got cancelled --> ignore
        return;
      LOGGING_LOG(m_logger, severity::warning)
          << "timer async wait error: " << error.message();
      return;
    }

    read();
  });
}

bool wolf::ee895_reader::send_command(uint8_t command) {
  return m_reader_writer.write_byte(command);
}
