#include "pppd_output_checker.hpp"
#include <boost/asio/read_until.hpp>

using namespace logging;

pppd_output_checker::pppd_output_checker(boost::asio::io_context& context)
    : m_logger{"ppd_output_checker"}, context(context), async_pipe(context) {}

void pppd_output_checker::start() { read(); }

void pppd_output_checker::reset() {
  boost::system::error_code ignore_error;
  async_pipe.close(ignore_error);
  async_pipe = boost::process::async_pipe(context);
}

boost::process::async_pipe& pppd_output_checker::get_pipe() {
  return async_pipe;
}

void pppd_output_checker::read() {
  static const char new_line{'\n'};
  boost::asio::async_read_until(
      async_pipe, boost::asio::dynamic_buffer(buffer), new_line,
      [this](const auto& error, const auto transferred) {
        handle(error, transferred);
      });
}

void pppd_output_checker::handle(const boost::system::error_code& error,
                                 const std::size_t transferred) {
  const std::string line = buffer.substr(0, transferred);
  buffer.erase(0, transferred);
  LOGGING_LOG(m_logger, severity::normal)
      << "pppd_output_checker, pppd log:" << line << std::flush;
  check(line);
  if (error.value() == boost::asio::error::misc_errors::eof) return;
  if (error) {
    LOGGING_LOG(m_logger, severity::normal)
        << "pppd_output_checker::handle, error:" << error.message();
    return;
  }
  read();
}

static bool contains_ipcp_timeout(const std::string_view line) {
  static std::string search{"IPCP: timeout sending Config-Requests"};
  return (line.find(search) != std::string::npos);
}

static bool contains_connect_script_failed(const std::string_view line) {
  static std::string search{"Connect script failed"};
  return (line.find(search) != std::string::npos);
}

void pppd_output_checker::check(const std::string_view line) {
  if (contains_ipcp_timeout(line))
    LOGGING_LOG(m_logger, severity::normal)
        << "pppd_output_checker, found ipcp timeout!";
  else if (contains_connect_script_failed(line))
    LOGGING_LOG(m_logger, severity::normal)
        << "pppd_output_checker, found connect script failure!";
  else
    return;

  signal_problem_detected();
}
