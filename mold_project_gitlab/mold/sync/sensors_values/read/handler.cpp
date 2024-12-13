#include "handler.hpp"

using namespace mold::sync::sensors_values::read;
using namespace mold::sync::sensors_values;

handler::handler(parser &parser_, wolf::sensor_value_handler &values_)
    : m_logger{"sync::sensors_values::read::handler"},
      m_parser(parser_),
      m_values(values_) {
  connect_signals();
}

void handler::connect_signals() {
  m_signal_connections.emplace_back(m_parser.signal_parsed.connect(
      [this](auto &response, const auto &values) {
        handle_message_with_catch(response, values);
      }));
}

void handler::handle_message_with_catch(
    wolf::network::response::send_helper &response,
    const handler::values_type &values) {
  try {
    handle_message(response, values);
  } catch (const std::runtime_error &error) {
    try {
      std::stringstream description;
      description << "could not handle message, error:" << error.what();
      const auto description_string = description.str();
      LOGGING_LOG(m_logger, logging::severity::error) << description_string;
      response.failure(description_string);
    } catch (...) {
    }
  }
}

void handler::handle_message(wolf::network::response::send_helper &response,
                             const handler::values_type &values) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "handle_message, values:" << values;
  m_values.handle_list(values);
  response.success();
}
