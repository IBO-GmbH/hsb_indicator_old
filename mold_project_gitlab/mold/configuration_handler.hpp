#ifndef MOLD_CONFIGURATION_HANDLER_HPP
#define MOLD_CONFIGURATION_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "mold/calculation_state.hpp"
#include "mold/configuration.hpp"
#include "mold/configuration_cache.hpp"
#include "wolf/key_value_cache.hpp"
#include "wolf/outdoor_sensor.hpp"

namespace mold {

class configuration_handler {
 public:
  configuration_handler(configuration_cache& database,
                        wolf::key_value_cache& key_value_cache);
  configuration_handler(const configuration_handler&) = delete;

  virtual ~configuration_handler() = default;
  virtual void add(const configuration& to_add);
  void update(const configuration& to_update);
  void remove(const wolf::types::uuid_array& id);
  void resync();

  virtual void set_outdoor_sensor(const wolf::outdoor_sensor& sensor_id);
  const wolf::outdoor_sensor& get_outdoor_sensor() const;

  using configurations = std::vector<configuration>;
  const configurations& get_all() const;
  boost::optional<configuration> get(const wolf::types::uuid_array& id) const;
  configuration get_or_throw(const wolf::types::uuid_array& id) const;
  boost::optional<configuration> get_by_room_id(
      const wolf::types::uuid_array& room_id) const;
  configuration get_by_room_id_or_throw(
      const wolf::types::uuid_array& room_id) const;
  bool remove_by_room_id(const wolf::types::uuid_array& room_id);

  boost::signals2::signal<void(const configuration&)> signal_added;
  boost::signals2::signal<void(const configuration&)> signal_updated;
  boost::signals2::signal<void(const wolf::types::uuid_array&)> signal_removed;
  boost::signals2::signal<void(const wolf::outdoor_sensor& old,
                               const wolf::outdoor_sensor& new_)>
      signal_outdoor_changed;

 private:
  void load_all_from_database();

  logging::logger m_logger;
  configuration_cache& m_database;
  wolf::key_value_cache& m_key_value_cache;
  wolf::outdoor_sensor m_outdoor_sensor;
  configurations m_configurations;
};
}  // namespace mold

#endif  // MOLD_CONFIGURATION_HANDLER_HPP
