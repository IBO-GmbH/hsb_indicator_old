#include "sensor_profile_helper.hpp"
#include "log/logger.hpp"
#include "wolf/network/sensor_id_helper.hpp"
#include "wolf/network/sensor_value_type_helper.hpp"

using namespace wolf::network;

wolf::sensor_profile sensor_profile_helper::cast_to_wolf(
    const Clima::SensorProfile &to_cast) {
  sensor_value_type_helper helper;
  const sensor_id id(to_cast.Id.Id, helper.cast_to_wolf(to_cast.Type),
                     to_cast.Id.ForeignSensorOwner);
  const sensor_profile result{id, sensor_type::foreign, {}};
  return result;
}

Clima::SensorMetadata sensor_profile_helper::cast_to_network(
    const wolf::sensor_metadata &to_cast) {
  Clima::SensorMetadata metadata;
  metadata.Type = to_cast.type;
  metadata.MinValue = to_cast.min_value;
  metadata.MaxValue = to_cast.max_value;
  metadata.Properties = to_cast.properties;
  return metadata;
}

Clima::SensorProfile sensor_profile_helper::cast_to_network(
    const wolf::sensor_profile &to_cast) {
  sensor_value_type_helper helper;
  Clima::SensorProfile result;
  result.Type = helper.cast_to_network(to_cast.id);
  result.Id = sensor_id_helper().cast_to_network(to_cast.id);
  result.Metadata = cast_to_network(to_cast.metadata);
  return result;
}
