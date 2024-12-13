#ifndef WOLF_SHT25_READER_HPP
#define WOLF_SHT25_READER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class sht25_reader {
 public:
  using read_buffer = std::array<std::uint8_t, 3>;

  sht25_reader(boost::asio::io_service& service);

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_async(int read_interval);

  boost::signals2::signal<void(const read_buffer& temperature,
                               const read_buffer& humidity)>
      signal_read;
  boost::signals2::signal<void(const read_buffer& temperature)>
      signal_read_temperature;
  boost::signals2::signal<void(const read_buffer& humidity)>
      signal_read_humidity;

  virtual ~sht25_reader() = default;

 private:
  bool do_soft_reset_sync();
  void print_bytes_read();
  bool print_user_register();

  bool check_open(const char check_byte);
  bool read_temperature();
  void handle_read_temperature(const boost::system::error_code& error);
  bool read_humidity();
  void handle_read_humidity(const boost::system::error_code& error);

 private:
  logging::logger m_logger;

  boost::asio::io_service& m_service;
  boost::asio::deadline_timer m_timer;
  i2c::device m_file{-1};
  int m_read_interval;
  i2c::read_write reader_writer;

  read_buffer m_read_buffer_temperature;
  read_buffer m_read_buffer_humidity;
};
}  // namespace wolf

#endif  // WOLF_SHT25_READER_HPP
