#ifndef WOLF_SERIAL_READER_HPP
#define WOLF_SERIAL_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/signals2/signal.hpp>

#include "log/logger.hpp"
#include "types.hpp"

namespace wolf {

class serial_interface : boost::noncopyable {
 public:
  using read_buffer = std::array<std::uint8_t, 128>;

  serial_interface(boost::asio::io_service& service);

  bool open(const std::string& port, const std::uint32_t& baud);

  void read();
  bool read(types::data& data);
  void read_async();

  bool write(const types::data& data);
  void write_async(const types::data& data);

  void close();

  boost::signals2::signal<void(const read_buffer&, uint32_t)> signal_read;

 private:
  void handle_read_async(const boost::system::error_code& error,
                         std::size_t bytes_transferred);
  void handle_write_async(const boost::system::error_code& error,
                          std::size_t bytes_transferred);
  void print_bytes_read(std::size_t bytes_transferred);
  void write_from_cache();

 private:
  logging::logger m_logger;

  boost::asio::io_service& m_service;
  boost::asio::serial_port m_serial;

  read_buffer m_read_buffer;
  bool m_writing;
  types::data m_write_buffer;
};
}

#endif  // WOLF_SERIAL_READER_HPP
