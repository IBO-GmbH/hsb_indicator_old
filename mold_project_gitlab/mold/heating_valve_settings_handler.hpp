#ifndef MOLD_HEATING_VALVE_SETTINGS_HANDLER_HPP
#define MOLD_HEATING_VALVE_SETTINGS_HANDLER_HPP

#include "mold/heating_valve_settings_cache.hpp"
#include "wolf/room.hpp"

namespace mold {

class heating_valve_settings_handler {
 public:
  heating_valve_settings_handler(heating_valve_settings_cache &cache);
  void set_options(const heating_valve_settings &options);
  using settings_list = std::vector<heating_valve_settings>;
  settings_list get_options_of_all_rooms();
  std::optional<heating_valve_settings> get_options_for_room(
      const wolf::room &room);
  void remove_settings(const wolf::types::uuid_array &room_id);

 private:
  void add_options(const heating_valve_settings &to_add);
  void update_options(const heating_valve_settings &to_update,
                      settings_list::iterator &found);
  settings_list::const_iterator find(const wolf::types::uuid_array &id) const;
  settings_list::iterator find(const wolf::types::uuid_array &id);

 private:
  logging::logger m_logger;
  heating_valve_settings_cache &m_cache;

  settings_list m_options;
};
}  // namespace mold

#endif  // MOLD_HEATING_VALVE_SETTINGS_HANDLER_HPP
