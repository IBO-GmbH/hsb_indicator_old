#include "parser.hpp"
#include "mold/network/building_type_helper.hpp"
#include "mold/network/substrate_type_helper.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/network/sensor_profile_helper.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::box_settings::read;

parser::parser(wolf::network::bond_parser& bond_parser_,
               wolf::network::response::sender& response)
    : m_logger{"sync::box_settings::read::parser"}, m_response(response) {
  m_signal_connections.emplace_back(bond_parser_.signal_message.connect(
      [this](const auto& message) { return handle_with_catch(message); }));
}

bool parser::handle_with_catch(const Clima::Message& message) {
  if (message.Type != ::Clima::Enums::MessageType::BoxSettings) return false;
  try {
    return handle(message);
  } catch (const std::runtime_error& error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle. error:" << error.what();
  }
  return false;
}

using sensor_map =
    std::map< ::Clima::Enums::SensorType, ::Clima::SensorProfile>;

static std::optional<wolf::sensor_id> cast_sensor_from_map(
    const sensor_map& from, const ::Clima::Enums::SensorType type) {
  const auto found = from.find(type);
  if (found == from.cend()) return {};
  const auto casted =
      wolf::network::sensor_profile_helper().cast_to_wolf(found->second);
  return casted.id;
};

bool parser::handle(const Clima::Message& message) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "handle";
  wolf::network::bond_deserializer deserializer;
  const Clima::BoxSettings settings =
      deserializer.parse_message<Clima::BoxSettings>(message.Entry);
  entity result;
  result.threshold_factor = settings.ThresholdFactor;
  result.building_type_ = network::building_type_helper().cast_from_network(
      settings.BuildingTypeSetting);
  result.substrate_type_ = network::substrate_type_helper().cast_from_network(
      settings.SubstrateTypeSetting);
  {
    const auto humidity = cast_sensor_from_map(
        settings.Sensors, ::Clima::Enums::SensorType::Humidity_Percent);
    result.outdoor_sensor.humidity = humidity.value_or(wolf::sensor_id{});
  }
  {
    const auto temperature = cast_sensor_from_map(
        settings.Sensors, ::Clima::Enums::SensorType::Temperature_Celsius);
    result.outdoor_sensor.temperature = temperature.value_or(wolf::sensor_id{});
  }
  const auto work_uuid = wolf::uuid_helper().vector_to_array(settings.WorkUUID);
  wolf::network::response::send_helper answer{m_response, work_uuid};
  signal_parsed(answer, result);
  return true;
}
