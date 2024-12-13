#include "bond_parser.hpp"
#include <bond/core/bond.h>

using namespace wolf::network;
using namespace logging;

bond_parser::bond_parser(wolf::websocket_sync &source)
    : m_logger("bond_parser") {
  m_connection_message = source.signal_message.connect(
      [this](const auto &message) { handle_message_with_catch(message); });
}

void bond_parser::handle_message_with_catch(const std::string &to_parse) {
  try {
    handle_message(to_parse);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle message. error:" << error.what();
  }
}

void bond_parser::handle_message(const std::string &to_parse) {
  if (to_parse.empty()) {
    LOGGING_LOG(m_logger, severity::error)
        << "network_parser::handle_message, to_parse.empty()";
    return;
  }
  const bond::InputBuffer input(to_parse.data(), to_parse.size());
  using reader = bond::CompactBinaryReader<bond::InputBuffer>;
  const reader reader_(input);
  Clima::Message message;
  bond::Deserialize(reader_, message);

  LOGGING_LOG(m_logger, severity::verbose)
      << "received an message with type:" << std::dec
      << static_cast<int>(message.Type);

  if (signal_message(message)) return;

  LOGGING_LOG(m_logger, severity::error)
      << "received an message that couldn't be parsed!"
         " type:"
      << std::dec << static_cast<int>(message.Type);
}
