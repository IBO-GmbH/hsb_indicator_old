#ifndef WOLF_SPGC3_HANDLER_HPP
#define WOLF_SPGC3_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include "log/logger.hpp"
#include "program_options_parser.hpp"
#include "sensor_filter.hpp"
#include "sensor_value.hpp"
#include "sgpc3_parser.hpp"
#include "sgpc3_reader.hpp"

namespace wolf {

class sgpc3_handler {
 public:
  using duration = std::chrono::steady_clock::duration;
  using seconds = std::chrono::seconds;

  sgpc3_handler(boost::asio::io_service &service, sgpc3_reader &reader,
                const wolf::config &program_options);
  void start_reading();
  void close();
  void resend_signal();

  boost::signals2::signal<void(const sensor_value &)> signal_value;

 private:
  void read();
  void handle_read(const sgpc3_reader::read_buffer &buffer,
                   const sensor_id &id);

 private:
  logging::logger m_logger;
  boost::asio::steady_timer m_timer_read;
  std::chrono::steady_clock::time_point m_next_read;
  sgpc3_reader &m_reader;
  sgpc3_parser m_parser;

  sensor_filter m_filter_handler;
  low_pass_filter m_filter_voc;

  const duration time_to_measure = seconds(2);
};
}  // namespace wolf

#endif  // WOLF_SPGC3_HANDLER_HPP
