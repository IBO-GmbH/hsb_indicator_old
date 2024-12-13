#ifndef WOLF_EE895_READER_HPP
#define WOLF_EE895_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class ee895_reader {
 public:
  using read_buffer = std::array<std::uint8_t, 8>;
  using seconds = std::chrono::seconds;

  ee895_reader(boost::asio::io_service& service);

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_async(seconds read_interval);

  boost::signals2::signal<void(const read_buffer& values)> signal_read;

  virtual ~ee895_reader() = default;

 private:
  bool check_open();
  bool read();
  void handle_read(const boost::system::error_code& error);
  void trigger_read();
  bool send_command(std::uint8_t command);

 private:
  logging::logger m_logger;

  boost::asio::steady_timer m_timer;
  i2c::device m_file{-1};
  seconds m_read_interval;
  i2c::read_write m_reader_writer;

  static constexpr std::uint8_t m_request_measurement = 0x00;
  const seconds m_min_read_interval{15};
};
}  // namespace wolf

#endif  // WOLF_EE895_READER_HPP
