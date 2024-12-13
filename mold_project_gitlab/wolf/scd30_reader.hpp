#ifndef WOLF_SCD30_READER_HPP
#define WOLF_SCD30_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/signals2/signal.hpp>
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class scd30_reader {
 public:
  scd30_reader();

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool trigger_continuous_measurement(int seconds = 2);
  bool set_asc_active(const bool active);

  using read_buffer = std::array<std::uint8_t, 3>;
  std::optional<read_buffer> get_data_ready_status();
  using read_data_buffer = std::array<std::uint8_t, 18>;
  std::optional<scd30_reader::read_data_buffer> get_data();

  virtual ~scd30_reader() = default;

 private:
  bool check_open();
  bool send_command(std::uint16_t command);
  bool send_command_with_argument(const uint16_t command,
                                  const uint16_t argument);

  logging::logger m_logger;

  i2c::device m_file{-1};
  i2c::read_write m_reader_writer;

  static constexpr std::uint16_t m_get_data_ready_status = 0x0202;
  static constexpr std::uint16_t m_get_data = 0x0300;
  static constexpr std::uint16_t m_trigger_continuous_measurement = 0x0010;
  static constexpr std::uint16_t m_continuous_measurement_interval = 0x4600;
  static constexpr std::uint16_t m_activate_asc = 0x5306;
};
}  // namespace wolf

#endif  // WOLF_SCD30_READER_HPP
