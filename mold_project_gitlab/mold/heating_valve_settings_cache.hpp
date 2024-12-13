#ifndef MOLD_HEATING_VALVE_SETTINGS_CACHE_HPP
#define MOLD_HEATING_VALVE_SETTINGS_CACHE_HPP

#include "heating_valve_settings.hpp"
#include "wolf/table_sync_helper.hpp"

namespace mold {

class heating_valve_settings_cache {
 public:
  heating_valve_settings_cache(wolf::database& database);

  void add(const heating_valve_settings& to_add);
  void update(const heating_valve_settings& to_update);
  void remove(const wolf::types::uuid_array& to_remove);

  using settings_list = std::vector<heating_valve_settings>;
  settings_list get_all();

 private:
  logging::logger m_logger;
  using sync = wolf::table_sync_helper<heating_valve_settings, std::string>;
  sync m_sync;
};
}  // namespace mold

#endif  // MOLD_HEATING_VALVE_SETTINGS_CACHE_HPP
