#include "sender.hpp"
#include "mold/network/building_type_helper.hpp"
#include "mold/network/substrate_type_helper.hpp"
#include "wolf/network/sensor_profile_helper.hpp"
#include "wolf/network/sensor_value_type_helper.hpp"
#include "wolf/sensor_id_rssi.hpp"

using namespace mold::sync::box_settings::write;

sender::sender(wolf::network::bond_sender &sender,
               wolf::network::response::simple &response,
               wolf::profile_handler &profile_handler, const bool send_rssi)
    : m_logger{"sync::box_settings::write::sender"},
      m_sender(sender),
      m_response(response),
      m_profile_handler(profile_handler),
      m_send_rssi(send_rssi) {}

void sender::add_sensor_to_map(sensor_map &to_add_to,
                               const wolf::sensor_id &to_add) {
  using namespace wolf::network;
  if (!to_add.is_set()) return;
  const auto casted_type = sensor_value_type_helper().cast_to_network(to_add);
  auto profile = m_profile_handler.get(to_add);
  const auto casted_profile = sensor_profile_helper().cast_to_network(profile);
  to_add_to[casted_type] = casted_profile;
}

void sender::send(const entity &to_send,
                  const wolf::network::response::simple::callback &callback) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "send, entity:" << to_send;
  ::Clima::BoxSettings result;
  result.ThresholdFactor = to_send.threshold_factor;
  result.BuildingTypeSetting =
      network::building_type_helper().cast_to_network(to_send.building_type_);
  result.SubstrateTypeSetting =
      network::substrate_type_helper().cast_to_network(to_send.substrate_type_);
  const auto &outdoor_sensor = to_send.outdoor_sensor;
  add_sensor_to_map(result.Sensors, outdoor_sensor.temperature);
  add_sensor_to_map(result.Sensors, outdoor_sensor.humidity);
  if (m_send_rssi)
    if (outdoor_sensor.is_set()) {
      const auto sensor_id_value = outdoor_sensor.temperature.is_set()
                                       ? outdoor_sensor.temperature
                                       : outdoor_sensor.humidity;
      const auto id_rssi =
          wolf::sensor_id_rssi::create_from_sensor_id(sensor_id_value);
      add_sensor_to_map(result.Sensors, id_rssi);
    }
  const auto work_uuid =
      m_sender.send(result, ::Clima::Enums::MessageType::BoxSettings);
  m_response.register_work_uuid(work_uuid, callback);
}
