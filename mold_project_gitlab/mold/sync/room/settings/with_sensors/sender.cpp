#include "sender.hpp"
#include "mold/network/building_type_helper.hpp"
#include "mold/network/substrate_type_helper.hpp"
#include "wolf/network/sensor_profile_helper.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold::sync::room::settings::with_sensors;

sender::sender(wolf::network::bond_sender &sender,
               wolf::network::response::simple &response)
    : m_logger{"sync::room::settings::with_sensors::sender"},
      m_sender(sender),
      m_reponse(response) {}

void sender::send(const sender::information &information_,
                  const wolf::network::response::simple::callback &callback) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "send information_:" << information_;
  Clima::RoomSettingsWithSensors result;
  wolf::network::sensor_profile_helper profile_helper;
  std::vector<Clima::SensorProfile> profiles_casted;
  std::transform(information_.profiles.cbegin(), information_.profiles.cend(),
                 std::back_inserter(profiles_casted), [&](const auto &item) {
                   return profile_helper.cast_to_network(item);
                 });
  for (const auto &profile_casted : profiles_casted)
    result.Sensors[profile_casted.Type] = profile_casted;
  result.Name = information_.name;
  result.RoomUUID = wolf::uuid_helper().array_to_vector(information_.room);
  result.ThresholdFactor = information_.threshold_factor;
  result.BuildingTypeSetting = network::building_type_helper().cast_to_network(
      information_.building_type_);
  result.SubstrateTypeSetting =
      network::substrate_type_helper().cast_to_network(
          information_.substrate_type_);
  const auto work_uuid =
      m_sender.send(result, Clima::Enums::MessageType::RoomSettingsWithSensors);
  m_reponse.register_work_uuid(work_uuid, callback);
}

std::ostream &mold::sync::room::settings::with_sensors::operator<<(
    std::ostream &out, const sender::information &to_print) {
  return out << "{room:" << to_print.room << ", name:" << to_print.name
             << ", building_type:" << to_print.building_type_
             << ", threshold_factor:" << to_print.threshold_factor
             << ", profiles:" << to_print.profiles << "}";
}
