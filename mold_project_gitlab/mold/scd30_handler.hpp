#ifndef MOLD_SCD30_HANDLER_HPP
#define MOLD_SCD30_HANDLER_HPP

#include "wolf/scd30_handler.hpp"
#include "wolf/scd30_parser.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold {

class scd30_handler {
 public:
  scd30_handler(wolf::scd30_handler& handler,
                wolf::sensor_value_handler& value_handler);

  void handle_values(const wolf::scd30_handler::values_type& values);

 private:
  logging::logger m_logger;
  wolf::sensor_value_handler& m_value_handler;
};
}  // namespace mold

#endif  // MOLD_SCD30_HANDLER_HPP
