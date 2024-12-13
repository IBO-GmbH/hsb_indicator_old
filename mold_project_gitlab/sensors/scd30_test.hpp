#ifndef SENSORS_SCD30_TEST_HPP
#define SENSORS_SCD30_TEST_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include "log/logger.hpp"
#include "wolf/scd30_parser.hpp"
#include "wolf/scd30_reader.hpp"

class scd30_test {
 public:
  scd30_test(boost::asio::io_service &service, wolf::scd30_reader &reader);
  ~scd30_test();

 private:
  void read_in_a_second();
  void read();

  logging::logger m_logger;
  boost::asio::steady_timer m_timer_read;
  wolf::scd30_reader &m_reader;
  wolf::scd30_parser m_parser;
};

#endif  // SENSORS_SCD30_TEST_HPP
