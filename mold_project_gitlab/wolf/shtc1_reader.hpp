#ifndef WOLF_SHTC1_READER_HPP
#define WOLF_SHTC1_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class shtc1_reader {
 public:
  using read_buffer = std::array<std::uint8_t, 6>;
  using seconds = std::chrono::seconds;

  shtc1_reader(boost::asio::io_service& service);

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_async(seconds read_interval);

  boost::signals2::signal<void(const read_buffer& values)> signal_read;

  virtual ~shtc1_reader() = default;

 private:
  bool do_soft_reset_sync();
  bool read();
  void handle_read(const boost::system::error_code& error);
  void trigger_read();
  bool get_id_register();
  bool send_command(std::uint16_t command);

 private:
  logging::logger m_logger;

  boost::asio::steady_timer m_timer;
  i2c::device m_file{-1};
  seconds m_read_interval;
  i2c::read_write m_reader_writer;

  static constexpr std::uint16_t m_soft_reset = 0x805d;
  const int m_wait_for_soft_reset{1};  // milliseconds (datasheet Table 5)
  static constexpr std::uint16_t m_measurement = 0x7866;
  const int m_wait_for_measurement{15};  // milliseconds (datasheet Table 5)
  static constexpr std::uint16_t m_id_register = 0xefc8;

  const seconds m_min_read_interval{4};
};
}  // namespace wolf

#endif  // WOLF_SHTC1_READER_HPP
