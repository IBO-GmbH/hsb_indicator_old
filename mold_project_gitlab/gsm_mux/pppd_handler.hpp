#ifndef PPPD_HANDLER_HPP
#define PPPD_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/process/child.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>
#include "log/logger.hpp"
#include "pppd_output_checker.hpp"

class pppd_handler {
 public:
  pppd_handler(boost::asio::io_service &service_, const std::string &isp_name,
               pppd_output_checker &output);

  void run();
  void stop();

 private:
  void retry();
  void handle_exit(const boost::system::error_code &error);
  void handle_retry_timeout(const boost::system::error_code &error);

 private:
  mutable logging::logger m_logger;
  boost::asio::io_service &service;
  const std::string isp_name;
  pppd_output_checker &output;
  boost::asio::signal_set wait_for_child{service, SIGCHLD};
  boost::asio::steady_timer retry_timer{service};
  std::unique_ptr<boost::process::child> child;
  bool shall_retry{false};
};

#endif  // PPPD_HANDLER_HPP
