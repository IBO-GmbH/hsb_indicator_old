#include "room_handler.hpp"
#include "sensor_id_enocean.hpp"
#include "wolf/sensor_id_rssi.hpp"
#include "wolf/sensor_type_helper.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_generator.hpp"

using namespace logging;
using namespace wolf;

room_handler::room_handler(room_cache &database, const bool rssi_enabled)
    : m_logger("room_handler"), m_database(database) {
  m_rooms = m_database.get_all();
  if (rssi_enabled)
    for (auto &room : m_rooms) {
      if (has_rssi_sensor(room)) continue;
      const auto found_sensor = find_enocean_sensor(room);
      if (found_sensor == room.sensors.cend()) continue;
      const auto rssi_id = sensor_id_rssi::create_from_sensor_id(*found_sensor);
      room.sensors.push_back(rssi_id);
      update(room);
    }
}

bool room_handler::add(const room &to_add) {
  if (find(to_add.id) != m_rooms.cend()) return false;

  LOGGING_LOG(m_logger, severity::verbose) << "add() room:" << to_add;

  m_rooms.push_back(to_add);
  m_database.add(to_add);
  signal_added(to_add);
  return true;
}

bool room_handler::update(const room &to_update) {
  LOGGING_LOG(m_logger, severity::verbose) << "update() room:" << to_update;
  auto found = find(to_update.id);
  if (found == m_rooms.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "can't update room because not found, id:" << to_update;
    return false;
  }

  if (*found == to_update) {
    LOGGING_LOG(m_logger, severity::normal)
        << "skipped update room because did not change, room:" << to_update;
    return true;
  }

  const auto sensors_before = generate_enocean_sensor_list(m_rooms);
  *found = to_update;
  m_database.update(to_update);
  LOGGING_LOG(m_logger, severity::normal) << "updated room:" << to_update;
  remove_unused_sensors(sensors_before);
  signal_updated(to_update);
  return true;
}

bool room_handler::remove(const types::uuid_array &id) {
  auto found = find(id);
  if (found == m_rooms.cend()) {
    LOGGING_LOG(m_logger, severity::error)
        << "can't remove room because not found, id:" << id;
    return false;
  }

  const auto sensors_before = generate_enocean_sensor_list(m_rooms);
  const auto sensors_to_remove = found->sensors;
  m_rooms.erase(found);
  m_database.remove(id);
  remove_unused_sensors(sensors_before);
  signal_removed(id);
  signal_remove_values_for_sensors(sensors_to_remove);
  return true;
}

const room_handler::rooms &room_handler::get_all() const { return m_rooms; }

const room &room_handler::get(const types::uuid_array &id) const {
  auto found = find(id);
  if (found == m_rooms.cend())
    throw std::runtime_error("room_handler::get: room not found");
  return *found;
}

const room_handler::rooms room_handler::get_all_rooms_with_sensor(
    const sensor_id &id) const {
  rooms all_rooms;
  std::copy_if(
      m_rooms.cbegin(), m_rooms.cend(), std::back_inserter(all_rooms),
      [&id](const room &room_) { return is_sensor_in_room(room_, id); });
  const auto new_end = std::unique(all_rooms.begin(), all_rooms.end());
  all_rooms.erase(new_end, all_rooms.cend());
  return all_rooms;
}

void room_handler::replace_one_or_add_sensor_in_all(const sensor_id &old,
                                                    const sensor_id &new_) {
  LOGGING_LOG(m_logger, severity::verbose) << "replace_one_or_add_sensor_in_all"
                                              " old:"
                                           << old << " new:" << new_;

  if (!old.is_set() && !new_.is_set()) return;

  for (auto room : m_rooms) {
    const auto found =
        std::find_if(room.sensors.begin(), room.sensors.end(),
                     [&old](const sensor_id &check) { return old == check; });
    if (found == room.sensors.end()) {
      room.sensors.push_back(new_);
    } else {
      if (!new_.is_set())
        room.sensors.erase(found);
      else
        *found = new_;
    }
    update(room);
  }
}

void room_handler::remove_unused_sensors(
    const room_handler::sensors &sensor_list) {
  sensors list_copy;
  std::copy_if(
      sensor_list.begin(), sensor_list.end(), std::back_inserter(list_copy),
      [](const auto &id) { return sensor_type_helper::is_enocean(id); });
  sensors_set id_esp3_list;
  std::transform(list_copy.begin(), list_copy.end(),
                 std::inserter(id_esp3_list, id_esp3_list.end()),
                 [](const auto &id) { return convert_sensor_id(id); });
  remove_unused_sensors(id_esp3_list);
}

void room_handler::remove_unused_sensors(
    const room_handler::sensors_set &sensor_list) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "check " << sensor_list.size() << " sensors if unused";
  auto sensors_after = generate_enocean_sensor_list(m_rooms);
  sensors_vector diff;
  std::copy_if(sensor_list.begin(), sensor_list.end(), std::back_inserter(diff),
               [&sensors_after](const types::id_esp3 &id_before) {
                 auto found = sensors_after.find(id_before);
                 return found == sensors_after.end();
               });
  if (diff.empty()) return;
  signal_remove_sensors(diff);
}

bool room_handler::has_rssi_sensor(const room &room_) {
  const auto found =
      std::find_if(room_.sensors.begin(), room_.sensors.end(),
                   [](const wolf::sensor_id &sensor) {
                     return sensor_type_helper::is_sensor_rssi(sensor);
                   });
  return found != room_.sensors.cend();
}

bool room_handler::is_sensor_in_room(const room &room_,
                                     const sensor_id &sensor) {
  const auto room_sensors = room_.sensors;
  return std::find_if(room_sensors.cbegin(), room_sensors.cend(),
                      [&sensor](const sensor_id &room_sensor_id) {
                        return room_sensor_id == sensor;
                      }) != room_sensors.cend();
}

room_handler::rooms::const_iterator room_handler::find(
    const types::uuid_array &id) const {
  return std::find_if(m_rooms.cbegin(), m_rooms.cend(),
                      [&id](const room &check) { return id == check.id; });
}

room_handler::rooms::iterator room_handler::find(const types::uuid_array &id) {
  return std::find_if(m_rooms.begin(), m_rooms.end(),
                      [&id](const room &check) { return id == check.id; });
}

room_handler::sensors_set room_handler::generate_enocean_sensor_list(
    const rooms &rooms_) {
  std::set<types::id_esp3> sensors;
  for (const auto &room : rooms_) {
    auto found = find_enocean_sensor(room);
    if (found == room.sensors.end()) continue;
    sensors.insert(convert_sensor_id(*found));
  }
  return sensors;
}

room_handler::sensors::const_iterator room_handler::find_enocean_sensor(
    const room &room_) {
  return std::find_if(room_.sensors.begin(), room_.sensors.end(),
                      [](const wolf::sensor_id &sensor) {
                        return sensor_type_helper::is_enocean(sensor);
                      });
}

types::id_esp3 room_handler::convert_sensor_id(const sensor_id &id) {
  return sensor_id_enocean::convert_to_esp3_id(id);
}
