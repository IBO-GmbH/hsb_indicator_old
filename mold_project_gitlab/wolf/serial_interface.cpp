#include "serial_interface.hpp"
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <sstream>

using namespace logging;
using namespace wolf;

serial_interface::serial_interface(boost::asio::io_service &service)
    : m_logger{"serial_interface"},
      m_service(service),
      m_serial(m_service),
      m_writing{false} {}

bool serial_interface::open(const std::string &port,
                            const std::uint32_t &baud) {
  LOGGING_LOG(m_logger, severity::normal)
      << "opening serial port, " << port << " with baud " << std::dec << baud;
  boost::system::error_code error;
  m_serial.open(port, error);
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not open serial port:" << std::dec << port
        << " error:" << error.message();
    return false;
  }
  m_serial.set_option(boost::asio::serial_port::baud_rate(baud));
  return true;
}

void serial_interface::read() {
  LOGGING_LOG(m_logger, severity::verbose) << "start sync reading";

  boost::system::error_code error;
  while (!error) {
    std::size_t read_bytes =
        m_serial.read_some(boost::asio::buffer(m_read_buffer), error);
    print_bytes_read(read_bytes);
    signal_read(std::ref(m_read_buffer), static_cast<unsigned int>(read_bytes));
  }
  LOGGING_LOG(m_logger, severity::error)
      << "read done because of an error:" << error.message();
}

bool serial_interface::read(types::data &data) {
  boost::system::error_code error;
  boost::asio::read(m_serial, boost::asio::buffer(data), error);
  if (!error) {
    return true;
  }
  LOGGING_LOG(m_logger, severity::error)
      << "could not finish synced read, because of, message:"
      << error.message();
  return false;
}

void serial_interface::read_async() {
  m_serial.async_read_some(
      boost::asio::buffer(m_read_buffer),
      std::bind(&serial_interface::handle_read_async, this,
                std::placeholders::_1, std::placeholders::_2));
}

bool serial_interface::write(const types::data &data) {
  boost::system::error_code error;
  boost::asio::write(m_serial, boost::asio::buffer(data), error);
  if (!error) {
    return true;
  }
  LOGGING_LOG(m_logger, severity::error)
      << "could not finish synced write, because of, message:"
      << error.message();
  return false;
}

void serial_interface::write_async(const types::data &data) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "write_async got called with data.size():" << data.size();
  std::copy(data.cbegin(), data.cend(), std::back_inserter(m_write_buffer));
  write_from_cache();
}

void serial_interface::close() { m_serial.close(); }

void serial_interface::handle_read_async(const boost::system::error_code &error,
                                         std::size_t bytes_transferred) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_read_async got called with error:" << error.message();
    return;
  }
  print_bytes_read(bytes_transferred);
  signal_read(std::cref(m_read_buffer),
              static_cast<unsigned int>(bytes_transferred));
  read_async();
}

void serial_interface::handle_write_async(
    const boost::system::error_code &error, std::size_t bytes_transferred) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_write_async got called with bytes_transferred:"
      << bytes_transferred;
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_write_async got called with error:" << error.message();
    return;
  }
  m_write_buffer.erase(
      m_write_buffer.begin(),
      m_write_buffer.begin() + static_cast<const int>(bytes_transferred));
  m_writing = false;
  write_from_cache();
}

void serial_interface::print_bytes_read(std::size_t bytes_transferred) {
  std::stringstream out;
  out << "some bytes got read, count:" << bytes_transferred
      << " data:" << std::hex;
  std::copy_n(m_read_buffer.cbegin(), bytes_transferred,
              std::ostream_iterator<int>(out, " "));
  LOGGING_LOG(m_logger, severity::verbose) << out.str();
}

void serial_interface::write_from_cache() {
  if (m_writing) {
    return;
  }
  if (m_write_buffer.empty()) {
    return;
  }
  m_writing = true;
  m_serial.async_write_some(
      boost::asio::buffer(m_write_buffer),
      std::bind(&serial_interface::handle_write_async, this,
                std::placeholders::_1, std::placeholders::_2));
}
