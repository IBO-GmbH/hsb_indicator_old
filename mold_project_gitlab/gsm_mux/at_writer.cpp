#include "at_writer.hpp"
#include <boost/asio/write.hpp>
#include "string_print_helper.hpp"

using namespace logging;

at_writer::at_writer(boost::asio::serial_port& serial_, const std::string& info)
    : m_logger{std::string("at_writer(") + info + std::string(")")},
      serial(serial_) {}

void at_writer::write(const std::string& to_send) {
  const std::string new_line("\r\n");
  const std::string to_write = to_send + new_line;

  const std::size_t wrote =
      boost::asio::write(serial, boost::asio::buffer(to_write));
  LOGGING_LOG(m_logger, severity::verbose)
      << "wrote:" << wrote
      << ", to_send:" << string_print_helper::from_at_string(to_send);
}
