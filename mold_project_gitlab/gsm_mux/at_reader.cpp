#include "at_reader.hpp"
#include <boost/asio/read_until.hpp>
#include "string_print_helper.hpp"

using namespace logging;

at_reader::at_reader(boost::asio::serial_port& serial_, const std::string& info)
    : m_logger{std::string("at_reader(") + info + std::string(")")},
      serial(serial_) {}

void at_reader::read_till_ok(
    const at_reader::callback_type& callback,
    const std::chrono::steady_clock::duration& timeout) {
  read("\r\nOK\r\n", callback, timeout);
}

void at_reader::read(const std::string& read_until,
                     const callback_type& callback,
                     const std::chrono::steady_clock::duration& timeout) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "reading or timeout: "
      << std::chrono::duration_cast<std::chrono::seconds>(timeout).count()
      << ", read_until:" << string_print_helper::from_at_string(read_until);
  async_timeout(timeout);
  async_read(read_until, callback);
}

std::string at_reader::get_result() {
  const std::string result = convert_streambuf_to_string();
  LOGGING_LOG(m_logger, severity::verbose)
      << "async read, result:" << string_print_helper::from_at_string(result)
      << ", hex:" << string_print_helper::as_hex(result);
  return result;
}

void at_reader::async_timeout(
    const std::chrono::steady_clock::duration& timeout) {
  timer.expires_from_now(timeout);
  timer.async_wait([this](const boost::system::error_code& error) {
    if (error == boost::asio::error::operation_aborted) return;
    handle_default_async_error(error);
    LOGGING_LOG(m_logger, severity::warning) << "timer timed out!";
    throw std::runtime_error(
        "at_reader::async_timeout: there was a read timeout");
  });
}

void at_reader::async_read(const std::string& read_until,
                           const callback_type& callback) {
  boost::asio::async_read_until(
      serial, buffer, read_until,
      [this, callback](const boost::system::error_code& error,
                       std::size_t read_count_) {
        read_count = read_count_;
        LOGGING_LOG(m_logger, severity::verbose)
            << " async_read_until, read_count:" << read_count_
            << ", error:" << error.message();
        if (error == boost::asio::error::operation_aborted) return;
        handle_default_async_error(error);
        timer.cancel();
        callback();
      });
}

std::string at_reader::convert_streambuf_to_string() {
  const std::string result(
      boost::asio::buffers_begin(buffer.data()),
      boost::asio::buffers_begin(buffer.data()) + static_cast<int>(read_count));
  buffer.consume(read_count);
  return result;
}

void at_reader::handle_default_async_error(
    const boost::system::error_code& error) const {
  if (!error) return;
  if (error == boost::asio::error::operation_aborted) return;
  LOGGING_LOG(m_logger, severity::warning)
      << "there was a non default error:" << error.message() << ", will throw.";
  throw boost::system::system_error(error);
}
