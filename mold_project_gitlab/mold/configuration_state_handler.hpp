#ifndef MOLD_CONFIGURATION_STATE_HANDLER_HPP
#define MOLD_CONFIGURATION_STATE_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold/configuration.hpp"
#include "mold/configuration_state.hpp"
#include "mold/mold_value.hpp"
#include "wolf/key_value_cache.hpp"

namespace mold {

class configuration_state_handler {
 public:
  struct config {
    float yellow_threshold;
    float red_threshold;
  };
  configuration_state_handler(wolf::key_value_cache& key_value_database,
                              const config config_);

  void add(const configuration& to_add);
  void update(const configuration& to_update);
  void remove(const wolf::types::uuid_array& id);
  using all_result =
      std::vector<std::pair<wolf::types::uuid_array, configuration_state>>;
  all_result get_all() const;
  std::optional<configuration_state> get_state_for_config(
      const configuration& configuration);

  void handle_mold_value(const mold_value& mold_value_);
  void handle_confirm(const wolf::types::uuid_array& config_id);

  boost::signals2::signal<void(const wolf::types::uuid_array&,
                               const configuration_state&)>
      signal_added;
  boost::signals2::signal<void(const wolf::types::uuid_array&,
                               const calculation_state&)>
      signal_state_changed;

 private:
  void save_all();
  void load_all();

  using container_entry = std::pair<configuration, configuration_state>;
  using container = std::vector<container_entry>;
  container::iterator find(const wolf::types::uuid_array& id);

  void handle_state_change(const container::iterator& changed);
  void handle_no_mold_value(const wolf::types::uuid_array& config_id,
                            const std::chrono::system_clock::time_point& now);
  void handle_mold_value(const wolf::types::uuid_array& config_id,
                         const float mold_value,
                         const std::chrono::system_clock::time_point& now);

  logging::logger m_logger;
  wolf::key_value_cache& m_key_value_database;
  config m_config;
  container m_states;
};
}  // namespace mold

#endif  // MOLD_CONFIGURATION_STATE_HANDLER_HPP
