#include "StatusListHelper.hpp"
#include "DataExchange.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace mold;

StatusListHelper::StatusListHelper(const DataExchange &dataExchange)
    : dataExchange(dataExchange) {}

StatusListHelper::StatusList StatusListHelper::getStatusList() const {
  StatusList result;
  const auto &rooms = dataExchange.get_rooms();
  for (const auto &room : rooms) {
    auto config = hasRoomMoldSetUp(room.id);
    addConfigurationToListIfValid(result, room.id, config);
    addMultipleSensors(result, room.id, room.sensors, StatusType::particles);
    addMultipleSensors(result, room.id, room.sensors,
                       StatusType::current_clamp);
    for (const auto &sensor : room.sensors)
      addSensorToListIfValid(result, room.id, sensor);
  }
  return result;
}

boost::optional<configuration> StatusListHelper::hasRoomMoldSetUp(
    const wolf::types::uuid_array &room) const {
  return dataExchange.get_configuration_by_room(room);
}

void StatusListHelper::addConfigurationToListIfValid(
    StatusList &result, const wolf::types::uuid_array &room,
    const boost::optional<configuration> &config) {
  if (!isConfigurationPageValid(config)) return;
  Status toAdd;
  toAdd.type = StatusType::configuration;
  toAdd.room = room;
  result.push_back(toAdd);
}

bool StatusListHelper::isConfigurationPageValid(
    const boost::optional<configuration> &config) {
  if (!config.is_initialized()) return false;
  return config->humidity.is_set() || config->temperature.is_set();
}

void StatusListHelper::addSensorToListIfValid(
    StatusListHelper::StatusList &result, const wolf::types::uuid_array &room,
    const wolf::sensor_id &sensor_id) {
  if (!isSensorGenericOrCo2(sensor_id)) return;
  Status toAdd;
  toAdd.room = room;
  toAdd.sensors.push_back(sensor_id);
  if (wolf::sensor_value_type_helper::is_co2(sensor_id))
    toAdd.type = StatusType::co2;
  else
    toAdd.type = StatusType::generic;
  result.push_back(toAdd);
}

// i think we destroyed generic sensors with the whitelisting... if an enocean
// sensor, which is not integrated yet (e.g. windspeed) is learned, the learning
// works without a problem, but the values page will not be displayed
bool StatusListHelper::isSensorGenericOrCo2(const wolf::sensor_id &sensor_id) {
  if (wolf::sensor_value_type_helper::is_co2(sensor_id) ||
      wolf::sensor_value_type_helper::is_voc(sensor_id) ||
      wolf::sensor_value_type_helper::is_air_pressure(sensor_id) ||
      wolf::sensor_value_type_helper::is_light(sensor_id) ||
      wolf::sensor_value_type_helper::is_supply_voltage(sensor_id))
    return true;  // ignore all others, ignored ones belong to configuration or
                  // need no ui
  return false;
}

void StatusListHelper::addMultipleSensors(
    StatusListHelper::StatusList &result, const wolf::types::uuid_array &room,
    const std::vector<wolf::sensor_id> &sensors, const StatusType &type) {
  if (std::none_of(sensors.cbegin(), sensors.cend(), [&](const auto &id) {
        if (type == StatusType::particles)
          return wolf::sensor_value_type_helper::is_particle(id);
        if (type == StatusType::current_clamp)
          return wolf::sensor_value_type_helper::is_electric_current(id) ||
                 wolf::sensor_value_type_helper::is_electric_power(id) ||
                 wolf::sensor_value_type_helper::is_electric_energy(id);
        return false;
      }))
    return;
  Status toAdd;
  toAdd.room = room;
  for (const auto &sensor_id : sensors) {
    if ((type == StatusType::particles &&
         wolf::sensor_value_type_helper::is_particle(sensor_id)) ||
        (type == StatusType::current_clamp &&
         (wolf::sensor_value_type_helper::is_electric_current(sensor_id) ||
          wolf::sensor_value_type_helper::is_electric_power(sensor_id) ||
          wolf::sensor_value_type_helper::is_electric_energy(sensor_id))))
      toAdd.sensors.push_back(sensor_id);
  }
  toAdd.type = type;
  result.push_back(toAdd);
}
