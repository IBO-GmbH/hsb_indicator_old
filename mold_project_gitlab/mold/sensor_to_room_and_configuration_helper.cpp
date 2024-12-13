#include "sensor_to_room_and_configuration_helper.hpp"
#include "mold/configuration.hpp"
#include "mold/sensor_id_evaluation_humidity.hpp"
#include "mold/sensor_id_sprout.hpp"
#include "wolf/room.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;
using namespace mold;

sensor_to_room_and_configuration_helper::
    sensor_to_room_and_configuration_helper()
    : m_logger{"sensor_to_room_and_configuration_helper"} {}

void sensor_to_room_and_configuration_helper::handle(
    const std::vector<wolf::sensor_id> &new_sensors_for_room, wolf::room &room_,
    configuration &configuration_, const bool outdoor_valid_for_config) {
  // remove in config set ids
  configuration_.temperature = wolf::sensor_id();
  configuration_.humidity = wolf::sensor_id();
  // rm ALL sensors in the room!
  room_.sensors.clear();
  // (re)set all sensors
  for (const auto &sensor : new_sensors_for_room)
    add_sensor_to_room_if_valid(room_, sensor);

  // set new config sensors
  {
    const auto found_temperature =
        wolf::sensor_value_type_helper::find_temperature_in_list(
            new_sensors_for_room);
    if (found_temperature) configuration_.temperature = found_temperature.get();
  }
  {
    const auto found_humidity =
        wolf::sensor_value_type_helper::find_humidity_in_list(
            new_sensors_for_room);
    if (found_humidity) configuration_.humidity = found_humidity.get();
  }

  // add mold sensors
  if (configuration_.temperature.is_set() && configuration_.humidity.is_set() &&
      outdoor_valid_for_config) {
    const wolf::sensor_id sprout = sensor_id_sprout::create(configuration_.id);
    room_.sensors.push_back(sprout);
    const wolf::sensor_id evaluation_humidity =
        sensor_id_evaluation_humidity::create(configuration_.id);
    room_.sensors.push_back(evaluation_humidity);
  }
}

void sensor_to_room_and_configuration_helper::add_sensor_to_room_if_valid(
    wolf::room &room_, const wolf::sensor_id &sensor_id) {
  if (!sensor_id.is_set()) return;
  // skip these when generating list, because they will be added later on
  if (wolf::sensor_value_type_helper::is_sprout(sensor_id) ||
      wolf::sensor_value_type_helper::is_evaluation_humidity(sensor_id))
    return;
  room_.sensors.push_back(sensor_id);
}
