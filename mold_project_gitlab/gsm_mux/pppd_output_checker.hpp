#ifndef PPPD_OUTPUT_CHECKER_HPP
#define PPPD_OUTPUT_CHECKER_HPP

#include <boost/process/async_pipe.hpp>
#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"

class pppd_output_checker {
 public:
  pppd_output_checker(boost::asio::io_context& context);

  void start();
  void reset();
  boost::signals2::signal<void()> signal_problem_detected;
  boost::process::async_pipe& get_pipe();

 private:
  void read();
  void handle(const boost::system::error_code& error,
              const std::size_t transferred);
  void check(const std::string_view line);

 private:
  mutable logging::logger m_logger;
  boost::asio::io_context& context;
  boost::process::async_pipe async_pipe;
  std::string buffer;
};

#endif  // PPPD_OUTPUT_CHECKER_HPP
