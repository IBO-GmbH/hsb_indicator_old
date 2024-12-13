#include "sensor_id_helper.hpp"

using namespace wolf::network;

Clima::SensorId sensor_id_helper::cast_to_network(
    const wolf::sensor_id &to_cast) {
  Clima::SensorId result;
  result.Id = to_cast.get_id();
  result.ForeignSensorOwner = to_cast.get_owner();
  return result;
}

wolf::sensor_id sensor_id_helper::cast_from_network(
    const Clima::SensorId &to_cast, const wolf::sensor_value_type type) {
  return wolf::sensor_id{to_cast.Id, type, to_cast.ForeignSensorOwner};
}
