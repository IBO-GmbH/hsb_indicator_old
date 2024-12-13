#include "command_handler.hpp"
#include "at_reader.hpp"

using namespace logging;

command_handler::command_handler(boost::asio::serial_port &serial_,
                                 boost::asio::io_service &service_)
    : m_logger{"command_handler"},
      serial(serial_),
      service(service_),
      writer(serial, "command_handler") {}

std::string command_handler::write_at_command_and_verify_response(
    const std::string &to_send,
    const std::chrono::steady_clock::duration &timeout) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "sending at command:" << to_send;
  writer.write(to_send);
  const auto answer = read_line_or_timeout("\r\nOK\r\n", timeout);
  if (answer.find("OK") != std::string::npos) return answer;
  throw std::runtime_error(
      "gsm_mux::write_at_command_and_verify_response: answer was not \"OK\"");
}

std::string command_handler::read_line_or_timeout(
    const std::string &read_until,
    const std::chrono::steady_clock::duration &timeout) {
  at_reader reader(serial, "command_handler");
  reader.read(read_until, [] {}, timeout);

  service.run();
  service.reset();

  return reader.get_result();
}
