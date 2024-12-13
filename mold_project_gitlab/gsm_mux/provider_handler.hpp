#ifndef PROVIDER_HANDLER_HPP
#define PROVIDER_HANDLER_HPP

#include <boost/signals2/connection.hpp>
#include "at_read_write.hpp"
#include "log/logger.hpp"
#include "online_checker.hpp"
#include "provider_list.hpp"
#include "provider_writer.hpp"

class provider_handler {
 public:
  provider_handler(boost::asio::io_service &service,
                   boost::asio::serial_port &serial,
                   online_checker &online_check);
  void run(boost::interprocess::managed_shared_memory &shared_memory);
  void close();

  boost::signals2::signal<void()> signal_failed;

 private:
  void handle_offline(int failure_counter);
  void handle_refreshed();
  void handle_provider_changed();
  void start_timer();
  std::string get_current_provider() const;

 public:
  online_checker &checker;
  provider_list list{read_write};

 private:
  mutable logging::logger m_logger;
  at_read_write read_write;
  std::string current_provider;
  boost::asio::steady_timer timer;

  boost::signals2::scoped_connection connection_signal_offline;
  boost::signals2::scoped_connection connection_signal_refresh;
  boost::signals2::scoped_connection connection_signal_provider_changed;
  provider_writer provider_writer_;
};

#endif  // PROVIDER_HANDLER_HPP
