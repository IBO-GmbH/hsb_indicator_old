#include "heating_valve_settings_handler.hpp"

mold::heating_valve_settings_handler::heating_valve_settings_handler(
    heating_valve_settings_cache &cache)
    : m_logger("heating_valve_settings_handler"), m_cache(cache) {
  m_options = m_cache.get_all();
}

void mold::heating_valve_settings_handler::set_options(
    const mold::heating_valve_settings &options) {
  settings_list::iterator found = find(options.room_id);
  if (found == m_options.end()) {
    add_options(options);
    return;
  }
  update_options(options, found);
}

mold::heating_valve_settings_handler::settings_list
mold::heating_valve_settings_handler::get_options_of_all_rooms() {
  return m_options;
}

std::optional<mold::heating_valve_settings>
mold::heating_valve_settings_handler::get_options_for_room(
    const wolf::room &room) {
  const auto found = find(room.id);
  if (found == m_options.cend()) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "heating_valve_settings_handler::get_options_for_room: settings for "
           "room not found, id:"
        << room.id;
    return std::optional<heating_valve_settings>();
  }
  return *found;
}

void mold::heating_valve_settings_handler::remove_settings(
    const wolf::types::uuid_array &room_id) {
  auto found = find(room_id);
  if (found == m_options.cend()) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "can't remove settings for room because not found, room has no "
           "heating valve, id:"
        << room_id;
    return;
  }

  m_options.erase(found);
  m_cache.remove(room_id);
}

void mold::heating_valve_settings_handler::add_options(
    const mold::heating_valve_settings &to_add) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "add heating_valve_settings:" << to_add;
  m_options.push_back(to_add);
  m_cache.add(to_add);
}

void mold::heating_valve_settings_handler::update_options(
    const mold::heating_valve_settings &to_update,
    mold::heating_valve_settings_handler::settings_list::iterator &found) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "update heating_valve_settings:" << to_update;
  if (*found == to_update) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "skipped update heating_valve_settings because did not change, "
           "settings:"
        << to_update;
    return;
  }

  *found = to_update;
  m_cache.update(to_update);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "updated heating_valve_settings:" << to_update;
}

mold::heating_valve_settings_handler::settings_list::const_iterator
mold::heating_valve_settings_handler::find(
    const wolf::types::uuid_array &id) const {
  return std::find_if(m_options.cbegin(), m_options.cend(),
                      [&id](const heating_valve_settings &check) {
                        return id == check.room_id;
                      });
}

mold::heating_valve_settings_handler::settings_list::iterator
mold::heating_valve_settings_handler::find(const wolf::types::uuid_array &id) {
  return std::find_if(m_options.begin(), m_options.end(),
                      [&id](const heating_valve_settings &check) {
                        return id == check.room_id;
                      });
}
