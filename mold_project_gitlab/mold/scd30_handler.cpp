#include "scd30_handler.hpp"
#include "wolf/sensor_id_scd30.hpp"

using namespace mold;

scd30_handler::scd30_handler(wolf::scd30_handler &handler,
                             wolf::sensor_value_handler &value_handler)
    : m_logger{"scd30_handler"}, m_value_handler(value_handler) {
  handler.signal_values.connect(
      [this](const auto &values) { handle_values(values); });
}

void scd30_handler::handle_values(
    const wolf::scd30_handler::values_type &values) {
  m_value_handler.handle_list(values);
}
