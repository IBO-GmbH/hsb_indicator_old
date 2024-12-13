#include "sender.hpp"
#include "wolf/network/sensor_id_helper.hpp"
#include "wolf/timestamp_helper.hpp"

using namespace mold::sync::sensors_values::write;

sender::sender(wolf::network::bond_sender &sender,
               wolf::network::response::simple &response)
    : m_logger{"sync::sensors_values::sender::write"},
      m_sender(sender),
      m_reponse(response) {}

static bool order_by_id(const wolf::sensor_value &first,
                        const wolf::sensor_value &second) {
  return first.id.get_id() < second.id.get_id();
}

static bool compare_id(const wolf::sensor_value &first,
                       const wolf::sensor_value &second) {
  return first.id.get_id() == second.id.get_id();
}

static ::Clima::SensorValue cast_sensor_value_to_network(
    const wolf::sensor_value &to_cast) {
  ::Clima::SensorValue result;
  result.Timestamp =
      wolf::timestamp_helper().cast_to_unix_seconds(to_cast.timestamp);
  result.Value = static_cast<std::int32_t>(to_cast.value * 10000.f);
  return result;
}

void sender::send(const std::vector<wolf::sensor_value> &to_send,
                  const wolf::network::response::simple::callback &callback) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "send, values:" << to_send;
  const Clima::SensorsValues result = cast_to_network(to_send);
  const auto work_uuid =
      m_sender.send(result, Clima::Enums::MessageType::SensorsValues);
  m_reponse.register_work_uuid(work_uuid, callback);
}

Clima::SensorsValues sender::cast_to_network(
    std::vector<wolf::sensor_value> to_send) {
  Clima::SensorsValues result;
  std::sort(to_send.begin(), to_send.end(), order_by_id);
  std::vector<wolf::sensor_value> unique;
  std::unique_copy(to_send.cbegin(), to_send.cend(), std::back_inserter(unique),
                   compare_id);
  for (const auto &key : unique) {
    const auto values_for_sensor_id =
        std::equal_range(to_send.cbegin(), to_send.cend(), key, order_by_id);
    ::Clima::SensorValues &current_sensor =
        result.SensorsAndValues.emplace_back();
    current_sensor.Sensor =
        wolf::network::sensor_id_helper().cast_to_network(key.id);
    std::transform(values_for_sensor_id.first, values_for_sensor_id.second,
                   std::back_inserter(current_sensor.Values),
                   cast_sensor_value_to_network);
  }
  return result;
}
