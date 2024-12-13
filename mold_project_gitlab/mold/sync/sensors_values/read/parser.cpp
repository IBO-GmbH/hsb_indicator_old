#include "parser.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/network/sensor_id_helper.hpp"
#include "wolf/timestamp_helper.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::sensors_values::read;

parser::parser(wolf::network::bond_parser &bond_parser_,
               wolf::network::response::sender &response,
               wolf::profile_handler &profiles)
    : m_logger{"sync::sensors_values::read::parser"},
      m_response(response),
      m_profiles(profiles) {
  m_signal_connections.emplace_back(bond_parser_.signal_message.connect(
      [this](const auto &message) { return handle_with_catch(message); }));
}

bool parser::handle_with_catch(const Clima::Message &message) {
  if (message.Type != ::Clima::Enums::MessageType::SensorsValues) return false;
  try {
    return handle(message);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle. error:" << error.what();
  }
  return false;
}

parser::values_type parser::cast_from_network(
    const std::vector<::Clima::SensorValues> &to_cast) {
  parser::values_type result;
  wolf::network::sensor_id_helper id_helper;
  for (const auto &id_values : to_cast) {
    const auto value_type = m_profiles.get_sensor_value_type(
        id_values.Sensor.Id, id_values.Sensor.ForeignSensorOwner);
    const auto id = id_helper.cast_from_network(id_values.Sensor, value_type);
    for (const auto &value : id_values.Values) {
      const auto value_casted = static_cast<float>(value.Value) / 10000.f;
      const auto timestamp =
          wolf::timestamp_helper().cast_seconds_to_chrono(value.Timestamp);
      const wolf::sensor_value value_result{id, value_casted, timestamp};
      result.push_back(value_result);
    }
  }
  return result;
};

bool parser::handle(const Clima::Message &message) {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "handle";
  wolf::network::bond_deserializer deserializer;
  const Clima::SensorsValues values =
      deserializer.parse_message<Clima::SensorsValues>(message.Entry);
  wolf::uuid_helper uuid_helper;
  const auto work_uuid = uuid_helper.vector_to_array(values.WorkUUID);
  wolf::network::response::send_helper answer{m_response, work_uuid};
  values_type result = cast_from_network(values.SensorsAndValues);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "parsed, values:" << result;
  signal_parsed(answer, result);
  return true;
}
