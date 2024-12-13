#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <string>
#include "at_writer.hpp"
#include "log/logger.hpp"

class command_handler {
 public:
  command_handler(boost::asio::serial_port &serial_,
                  boost::asio::io_service &service_);

  std::string write_at_command_and_verify_response(
      const std::string &to_send,
      const std::chrono::steady_clock::duration &timeout =
          std::chrono::seconds(90));
  std::string read_line_or_timeout(const std::string &read_until = "\r\nOK\r\n",
                                   const std::chrono::steady_clock::duration
                                       &timeout = std::chrono::seconds(90));

 private:
  mutable logging::logger m_logger;
  boost::asio::serial_port &serial;
  boost::asio::io_service &service;
  at_writer writer;
};

#endif  // COMMAND_HANDLER_HPP
