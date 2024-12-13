#ifndef MOLD_SYNC_SENSORS_VALUES_READ_HANDLER_HPP
#define MOLD_SYNC_SENSORS_VALUES_READ_HANDLER_HPP

#include "mold/sync/sensors_values/read/parser.hpp"
#include "mold/sync/sensors_values/write/handler.hpp"

namespace mold::sync::sensors_values::read {

class handler {
 public:
  handler(parser& parser_, wolf::sensor_value_handler& values_);

 private:
  void connect_signals();
  using values_type = std::vector<wolf::sensor_value>;
  void handle_message_with_catch(wolf::network::response::send_helper& response,
                                 const values_type& values);
  void handle_message(wolf::network::response::send_helper& response,
                      const values_type& values);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  parser& m_parser;
  wolf::sensor_value_handler& m_values;
};
}  // namespace mold::sync::sensors_values::read
#endif
