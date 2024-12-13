#ifndef MOLD_SHT25_HANDLER_HPP
#define MOLD_SHT25_HANDLER_HPP

#include "wolf/program_options_parser.hpp"
#include "wolf/sht25_handler.hpp"
#include "wolf/sht25_reader.hpp"

namespace mold {

class sht25_handler {
 public:
  sht25_handler(const wolf::config& program_options,
                wolf::sht25_reader& reader);
  bool start_reading();
  void close();
  void resend_signals();

  logging::logger m_logger;
  wolf::sht25_reader& m_reader;
  const wolf::low_pass_filter_creator filter_temperature;
  const wolf::low_pass_filter_creator filter_humidity;
  wolf::sht25_handler handler;
  int read_interval;
};
}  // namespace mold

#endif  // MOLD_SHT25_HANDLER_HPP
