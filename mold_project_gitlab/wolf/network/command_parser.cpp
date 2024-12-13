#include "command_parser.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/uuid_helper.hpp"

using namespace wolf::network;

command_parser::command_parser(wolf::network::bond_parser &parser,
                               response::sender &response)
    : m_logger{"network::command_parser"}, m_response(response) {
  m_connection_message = parser.signal_message.connect(
      [this](const auto &message) { return handle_with_catch(message); });
}

bool command_parser::handle_with_catch(const Clima::Message &message) {
  try {
    return handle(message);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle. error:" << error.what();
  }
  return false;
}

bool command_parser::handle(const Clima::Message &message) {
  if (message.Type != Clima::Enums::MessageType::Command) return false;
  LOGGING_LOG(m_logger, logging::severity::verbose) << "handle";
  bond_deserializer deserializer;
  const auto command =
      deserializer.parse_message<Clima::Command>(message.Entry);
  uuid_helper helper;
  const auto uuid_casted = helper.vector_to_array(command.WorkUUID);
  response::send_helper answer{m_response, uuid_casted};
  signal_parsed(answer, command.BashScript);
  return true;
}
