#ifndef WOLF_SHT85_READER_HPP
#define WOLF_SHT85_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class sht85_reader {
 public:
  using read_buffer = std::array<std::uint8_t, 6>;
  using seconds = std::chrono::seconds;

  sht85_reader(boost::asio::io_service& service);

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_async(seconds read_interval);

  boost::signals2::signal<void(const read_buffer& values)> signal_read;

  virtual ~sht85_reader() = default;

 private:
  bool do_soft_reset_sync();
  bool read();
  void handle_read(const boost::system::error_code& error);
  void trigger_read();
  bool get_status_register();
  bool send_command(std::uint16_t command);

 private:
  logging::logger m_logger;

  boost::asio::steady_timer m_timer;
  i2c::device m_file{-1};
  seconds m_read_interval;
  i2c::read_write m_reader_writer;

  static constexpr std::uint16_t m_soft_reset = 0x30a2;
  const int m_wait_for_soft_reset{2};  // milliseconds (datasheet Table 4)
  static constexpr std::uint16_t m_measurement = 0x2400;
  const int m_wait_for_measurement{
      16};  // milliseconds (datasheet Table 4, high repeatability)
  static constexpr std::uint16_t m_status_register = 0xf32d;

  const seconds m_min_read_interval{1};
};
}  // namespace wolf

#endif  // WOLF_SHT85_READER_HPP
