#ifndef WOLF_SGPC3_READER_HPP
#define WOLF_SGPC3_READER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "wolf/i2c/read_write.hpp"

namespace wolf {

class sgpc3_reader {
 public:
  sgpc3_reader(boost::asio::io_service& service);
  virtual ~sgpc3_reader() = default;

  virtual bool open(const std::string& device, const int slave_id);
  void close();

  bool read_data_async();

  using read_buffer = std::array<std::uint8_t, 3>;
  boost::signals2::signal<void(const read_buffer& data)> signal_read_data;

 private:
  bool open_file(const std::string& device, const int slave_id);
  bool check_open(std::array<std::uint8_t, 2>& to_send);
  bool send_command(std::uint16_t command);
  void handle_read_data(const boost::system::error_code& error);

  logging::logger m_logger;

  boost::asio::deadline_timer m_timer;
  i2c::device m_file{-1};
  i2c::read_write m_reader_writer;
};
}  // namespace wolf

#endif  // WOLF_SGPC3_READER_HPP
