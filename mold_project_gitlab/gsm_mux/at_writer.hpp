#ifndef AT_WRITER_HPP
#define AT_WRITER_HPP

#include <boost/asio/serial_port.hpp>
#include "log/logger.hpp"

class at_writer {
 public:
  at_writer(boost::asio::serial_port &serial_, const std::string &info);

  void write(const std::string &to_send);

 private:
  mutable logging::logger m_logger;
  boost::asio::serial_port &serial;
};

#endif  // AT_WRITER_HPP
