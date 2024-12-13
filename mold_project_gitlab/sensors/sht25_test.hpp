#ifndef SHT25_TEST_HPP
#define SHT25_TEST_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include "log/logger.hpp"
#include "wolf/sht25_parser.hpp"
#include "wolf/sht25_reader.hpp"

class sht25_test {
 public:
  sht25_test(boost::asio::io_service& service, wolf::sht25_reader& reader);
  ~sht25_test();

 private:
  logging::logger m_logger;
  boost::asio::deadline_timer m_timer_read;
  wolf::sht25_reader& m_reader;
  wolf::sht25_parser m_parser;
};

#endif  // SHT25_TEST_HPP
