#ifndef ONLINE_CHECKER_HPP
#define ONLINE_CHECKER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"

class online_checker {
 public:
  online_checker(boost::asio::io_service &service, const std::string &url);
  void run();
  void stop();

  boost::signals2::signal<void(int)> signal_offline;

 private:
  void start_timer();
  void handle_timeout(const boost::system::error_code &error);
  void handle_resolve(const boost::system::error_code &error,
                      const boost::asio::ip::tcp::resolver::iterator &resolved);
  void handle_connect(const boost::system::error_code &error);
  bool check_for_error(const boost::system::error_code &error);
  void handle_failure();

 private:
  mutable logging::logger m_logger;
  boost::asio::steady_timer timer;
  boost::asio::ip::tcp::resolver resolver;
  boost::asio::ip::tcp::socket socket;
  const std::string url;
  int counter_failure{};
};

#endif  // ONLINE_CHECKER_HPP
