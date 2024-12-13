#ifndef AT_READER_HPP
#define AT_READER_HPP

#include <boost/asio/serial_port.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include "log/logger.hpp"

class at_reader {
 public:
  using callback_type = const std::function<void()>;

  at_reader(boost::asio::serial_port &serial_, const std::string &info);
  void read_till_ok(const callback_type &callback,
                    const std::chrono::steady_clock::duration &timeout =
                        std::chrono::seconds(90));
  void read(const std::string &read_until, const callback_type &callback,
            const std::chrono::steady_clock::duration &timeout =
                std::chrono::seconds(90));
  std::string get_result();

 private:
  void async_timeout(const std::chrono::steady_clock::duration &timeout);
  void async_read(const std::string &read_until, const callback_type &callback);
  std::string convert_streambuf_to_string();
  void handle_default_async_error(const boost::system::error_code &error) const;

 private:
  mutable logging::logger m_logger;
  boost::asio::serial_port &serial;
  boost::asio::steady_timer timer{serial.get_io_service()};
  boost::asio::streambuf buffer;
  std::size_t read_count{};
};

#endif  // AT_READER_HPP
