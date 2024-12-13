#include "parser.hpp"
#include "mold/network/building_type_helper.hpp"
#include "mold/network/substrate_type_helper.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::room::settings;

std::ostream& mold::sync::room::settings::operator<<(
    std::ostream& out, const parser::information& to_print) {
  return out << "{room:" << to_print.room << ", name:" << to_print.name
             << ", building_type:" << to_print.building_type_
             << ", threshold_factor:" << to_print.threshold_factor << "}";
}

parser::parser(wolf::network::bond_parser& bond_parser_,
               wolf::network::response::sender& response)
    : m_logger{"sync::room::settings::parser"}, m_response(response) {
  m_signal_connections.emplace_back(bond_parser_.signal_message.connect(
      [this](const auto& message) { return handle_with_catch(message); }));
}

bool parser::handle_with_catch(const Clima::Message& message) {
  if (message.Type != ::Clima::Enums::MessageType::RoomSettings) return false;
  try {
    return handle(message);
  } catch (const std::runtime_error& error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle. error:" << error.what();
  }
  return false;
}

bool parser::handle(const Clima::Message& message) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "handle";
  wolf::network::bond_deserializer deserializer;
  const Clima::RoomSettings settings =
      deserializer.parse_message<Clima::RoomSettings>(message.Entry);
  wolf::uuid_helper uuid_helper;
  const auto work_uuid = uuid_helper.vector_to_array(settings.WorkUUID);
  const auto room = uuid_helper.vector_to_array(settings.RoomUUID);
  wolf::network::response::send_helper answer{m_response, work_uuid};
  const auto building_type = network::building_type_helper().cast_from_network(
      settings.BuildingTypeSetting);
  const auto substrate_type =
      network::substrate_type_helper().cast_from_network(
          settings.SubstrateTypeSetting);
  const information result{room, settings.Name, building_type,
                           settings.ThresholdFactor, substrate_type};
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "parsed, information:" << result;
  signal_parsed(answer, result);
  return true;
}
