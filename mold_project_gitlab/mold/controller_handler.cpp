#include "controller_handler.hpp"
#include "wolf/actor/sensor_profile.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace mold;

controller_handler::controller_handler(
    wolf::controller::controller_handler &controller_handler,
    wolf::actor::handler &actor_handler, wolf::room_handler &room_handler,
    wolf::sensor_value_handler &value_handler,
    configuration_handler &configuration_handler_,
    wolf::sensor_value_offset_handler &offset_handler,
    configuration_values_handler &config_values_handler,
    wolf::profile_handler &profile_handler)
    : m_controller_handler(controller_handler),
      m_actor_handler(actor_handler),
      m_room_handler(room_handler),
      m_value_handler(value_handler),
      m_configuration_handler(configuration_handler_),
      m_offset_handler(offset_handler),
      m_config_values_handler(config_values_handler),
      m_profile_handler(profile_handler) {
  set_up_controller_signals();
  set_up_room_signals();
  set_up_value_signals();
  set_up_configuration_signals();
}

void controller_handler::set_up_controller_signals() {
  // TODO a lot of signals seem to be needed because controller_handler dos not
  // know about room_hander. FIX?
  m_controller_handler.signal_load_actor.connect(
      [this](const wolf::actor::actor &to_load) {
        m_actor_handler.add(to_load);
      });
  m_actor_handler.signal_base_id_loaded.connect([this] {
    auto actors = m_actor_handler.get_all();
    for (const auto &actor : actors)
      m_actor_handler.handle_state_changed(actor, true);
  });
  m_controller_handler.signal_limits.connect(
      [this](const wolf::types::uuid_array &id) {
        auto room = m_room_handler.get(id);
        m_controller_handler.handle_control_loops(room, true);
      });
  m_controller_handler.signal_active_sates.connect(
      [this](const wolf::types::uuid_array &id) {
        auto room = m_room_handler.get(id);
        m_controller_handler.handle_control_loops(room, true);
      });
  m_controller_handler.signal_add_actor.connect(
      [this](const wolf::types::uuid_array &id,
             const wolf::actor::actor &to_add) {
        auto room = m_room_handler.get(id);
        const auto base_id = m_actor_handler.get_base_id();
        wolf::actor::sensor_profile::add_actor_switch_to_room(
            to_add, base_id, room, m_room_handler, m_profile_handler);
        m_actor_handler.add(to_add);
        m_controller_handler.handle_control_loops(room, true);
      });
  m_controller_handler.signal_remove_actor.connect(
      [this](const wolf::types::id_esp3 &actor_id) {
        m_actor_handler.remove(actor_id);
      });
  m_controller_handler.signal_all_actors_removed.connect(
      [this](const wolf::types::uuid_array &id) {
        auto room = m_room_handler.get(id);
        m_controller_handler.handle_control_loops(room, true);
      });
  m_controller_handler.signal_actor_state.connect(
      [this](const wolf::controller::controller_handler::actor_list_pointer
                 &actors) {
        for (auto &actor : actors)
          if (actor->has_value())
            m_actor_handler.handle_state_changed(actor->get(), false);
      });
}

void controller_handler::set_up_room_signals() {
  m_room_handler.signal_added.connect(
      [this](const wolf::room &room) { m_controller_handler.add(room.id); });
  m_room_handler.signal_updated.connect(
      [this](const wolf::room &room) { m_controller_handler.update(room.id); });
  m_room_handler.signal_removed.connect(
      [this](const wolf::types::uuid_array &room_id) {
        m_controller_handler.remove(room_id);
      });
}

void controller_handler::set_up_value_signals() {
  m_value_handler.signal_values.connect([this](const auto &values) {
    const auto sensor_ids = get_sensor_ids_from_values(values);
    const auto rooms = get_rooms_with_sensor(sensor_ids);
    for (const auto &room : rooms)
      m_controller_handler.handle_control_loops(room, false);
  });
  m_offset_handler.signal_set.connect([this](const auto &, const auto &offset) {
    const auto rooms = get_rooms_with_sensor({offset.id});
    for (const auto &room : rooms) {
      m_controller_handler.reset_ventilation(room.id);
      if (wolf::sensor_value_type_helper::is_temperature(offset.id))
        m_controller_handler.reset_heating(room.id);
      if (wolf::sensor_value_type_helper::is_humidity(offset.id))
        m_controller_handler.reset_dehumidifier(room.id);
    }
  });
  m_config_values_handler.signal_values_outdoor.connect(
      [this](const mold::configuration_values_outdoor &) {
        const auto rooms = m_room_handler.get_all();
        for (const auto &room : rooms)
          m_controller_handler.update_absolute_humidity_thresholds(room.id);
      });
}

void controller_handler::set_up_configuration_signals() {
  m_configuration_handler.signal_outdoor_changed.connect(
      [this](const wolf::outdoor_sensor &,
             const wolf::outdoor_sensor &outdoor) {
        m_controller_handler.set_outdoor_sensor(outdoor);
      });
}

std::vector<wolf::room> controller_handler::get_rooms_with_sensor(
    const std::vector<wolf::sensor_id> &sensors) const {
  const auto outdoor_result = handle_outdoor_sensor(sensors);
  if (outdoor_result.size() > 0) return outdoor_result;
  return handle_indoor_sensors(sensors);
}

std::vector<wolf::room> controller_handler::handle_indoor_sensors(
    const std::vector<wolf::sensor_id> &sensors) const {
  std::vector<wolf::room> affected_rooms;
  const auto all_rooms = m_room_handler.get_all();
  for (const auto &check_sensor : sensors)
    std::copy_if(all_rooms.cbegin(), all_rooms.cend(),
                 std::back_inserter(affected_rooms),
                 [&check_sensor](const wolf::room &room) {
                   return is_controller_sensor({check_sensor}) &&
                          is_sensor_used(room, check_sensor);
                 });
  remove_room_multiples(affected_rooms);
  return affected_rooms;
}

std::vector<wolf::room> controller_handler::handle_outdoor_sensor(
    const std::vector<wolf::sensor_id> &sensors) const {
  const auto outdoor_sensor = m_configuration_handler.get_outdoor_sensor();
  if (!outdoor_sensor.is_set()) return {};
  const auto found =
      std::find_if(sensors.cbegin(), sensors.cend(),
                   [&outdoor_sensor](const wolf::sensor_id &sensor) {
                     return sensor == outdoor_sensor.temperature ||
                            sensor == outdoor_sensor.humidity;
                   });
  if (found == sensors.cend()) return {};
  std::vector<wolf::room> affected_rooms;
  const auto all_rooms = m_room_handler.get_all();
  std::copy_if(all_rooms.cbegin(), all_rooms.cend(),
               std::back_inserter(affected_rooms), [](const wolf::room &room) {
                 return is_controller_sensor(room.sensors);
               });
  return affected_rooms;
}

bool controller_handler::is_sensor_used(const wolf::room &room,
                                        const wolf::sensor_id &check_sensor) {
  const auto found =
      std::find_if(room.sensors.cbegin(), room.sensors.cend(),
                   [&check_sensor](const wolf::sensor_id &sensor) {
                     return sensor == check_sensor;
                   });
  return found != room.sensors.cend();
}

void controller_handler::remove_room_multiples(
    std::vector<wolf::room> &affected_rooms) {
  if (affected_rooms.size() == 1) return;
  const auto new_end =
      std::unique(affected_rooms.begin(), affected_rooms.end(),
                  [](const wolf::room &room_1, const wolf::room &room_2) {
                    return room_1.id == room_2.id;
                  });
  affected_rooms.erase(new_end, affected_rooms.end());
}

std::vector<wolf::sensor_id> controller_handler::get_sensor_ids_from_values(
    const std::vector<wolf::sensor_value> &values) {
  std::vector<wolf::sensor_id> sensor_ids;
  std::transform(values.cbegin(), values.cend(), std::back_inserter(sensor_ids),
                 [](const wolf::sensor_value &value) { return value.id; });
  return sensor_ids;
}

bool controller_handler::is_controller_sensor(
    const std::vector<wolf::sensor_id> &sensors) {
  for (const auto &sensor : sensors) {
    if (wolf::sensor_value_type_helper::is_humidity(sensor) ||
        wolf::sensor_value_type_helper::is_temperature(sensor) ||
        wolf::sensor_value_type_helper::is_co2(sensor) ||
        wolf::sensor_value_type_helper::is_voc(sensor))
      return true;
    else
      continue;
  }
  return false;
}
