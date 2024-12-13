#include "room_creator.hpp"
#include "mold/sensor_to_room_and_configuration_helper.hpp"
#include "wolf/uuid_generator.hpp"

using namespace mold;

room_creator::room_creator(wolf::room_handler &rooms,
                           configuration_handler &configurations)
    : m_logger{"room_creator"},
      m_rooms(rooms),
      m_configurations(configurations) {}

room_creator::result room_creator::create(
    const wolf::types::uuid_array &room_id, const std::string &name,
    const std::vector<wolf::sensor_id> &sensors,
    const wolf::types::uuid_array &initial_id, const bool is_initial) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "create, name:" << name << ", sensors:" << sensors;

  auto new_room_id = room_id;
  if (new_room_id == wolf::types::uuid_array{}) {
    new_room_id = initial_id;
    if (!is_initial)
      // possibility is really low, but it may happen, that the uuid generator
      // generates the same id as the initial id. it may even happen, that it
      // generate it more than once, so generate a new one till it is different
      while (new_room_id == initial_id)
        new_room_id = wolf::uuid_generator::generate_as_array();
  }
  wolf::room room_{new_room_id, name, {}};
  const auto configuration_id = wolf::uuid_generator::generate_as_array();
  configuration configuration_{configuration_id, new_room_id};
  sensor_to_room_and_configuration_helper sensor_helper;
  sensor_helper.handle(
      sensors, room_, configuration_,
      m_configurations.get_outdoor_sensor().temperature.is_set());
  m_rooms.add(room_);
  m_configurations.add(configuration_);
  return {room_, configuration_};
}
