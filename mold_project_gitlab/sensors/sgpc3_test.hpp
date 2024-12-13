#ifndef SGPC3_TEST_HPP
#define SGPC3_TEST_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include "log/logger.hpp"
#include "wolf/sgpc3_parser.hpp"
#include "wolf/sgpc3_reader.hpp"

class sgpc3_test {
 public:
  sgpc3_test(boost::asio::io_service &service, wolf::sgpc3_reader &reader);
  ~sgpc3_test();

 private:
  void wait_and_read_again();

  logging::logger m_logger;
  boost::asio::steady_timer m_timer_read;
  std::chrono::steady_clock::time_point m_next_read;
  wolf::sgpc3_reader &m_reader;
  wolf::sgpc3_parser m_parser;
};

#endif  // SGPC3_TEST_HPP
