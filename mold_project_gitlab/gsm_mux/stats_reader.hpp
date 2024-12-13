#ifndef STATS_READER_HPP
#define STATS_READER_HPP

#include "log/logger.hpp"
#include "rssi_reader.hpp"

class stats_reader {
 public:
  stats_reader(boost::asio::io_service& service);

  void open(boost::interprocess::managed_shared_memory& shared_memory,
            const std::string& path = "/dev/gsmtty2");
  void run();
  void close();

 private:
  void get_next();
  void handle_timeout(const boost::system::error_code& error);
  void handle_read_rssi();

  mutable logging::logger m_logger;
  boost::asio::io_service& service;
  boost::asio::serial_port serial;
  boost::asio::steady_timer timer;
  at_read_write read_write;

  rssi_reader rssi;
};

#endif  // STATS_READER_HPP
