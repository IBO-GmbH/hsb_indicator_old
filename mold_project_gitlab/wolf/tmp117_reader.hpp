#ifndef WOLF_TMP117_READER_HPP
#define WOLF_TMP117_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class tmp117_reader {
 public:
  using read_buffer = std::array<std::uint8_t, 2>;
  using seconds = std::chrono::seconds;

  tmp117_reader(boost::asio::io_service& service);

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_async(seconds read_interval);

  boost::signals2::signal<void(const read_buffer& values)> signal_read;

  virtual ~tmp117_reader() = default;

 private:
  bool do_soft_reset_sync();
  bool read();
  void handle_read(const boost::system::error_code& error);
  void trigger_read();
  bool get_id_register();
  bool send_register_pointer(std::uint8_t register_pointer);
  bool send_command(std::uint16_t command);

 private:
  logging::logger m_logger;

  boost::asio::steady_timer m_timer;
  i2c::device m_file{-1};
  seconds m_read_interval;
  i2c::read_write m_reader_writer;

  static constexpr std::uint16_t m_id_register = 0x0f;
  static constexpr std::uint16_t m_config_register = 0x01;
  static constexpr std::uint16_t m_temp_register = 0x00;
  static constexpr std::uint16_t m_soft_reset = 0x0222;
  const int m_wait_for_soft_reset{2};  // milliseconds (datasheet Table 7.6)
  static constexpr std::uint16_t m_measurement =
      0x0c00;  // no averaging, single measurement
  const int m_wait_for_measurement{
      16};  // milliseconds (datasheet Chapter 7.3.2)

  const seconds m_min_read_interval{1};
};
}  // namespace wolf

#endif  // WOLF_TMP117_READER_HPP
