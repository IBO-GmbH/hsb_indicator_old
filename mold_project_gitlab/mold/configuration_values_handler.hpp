#ifndef MOLD_CONFIGURATION_VALUES_HANDLER_HPP
#define MOLD_CONFIGURATION_VALUES_HANDLER_HPP

#include "configuration.hpp"
#include "mold/configuration_values_outdoor.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/sensor_value.hpp"

namespace mold {

class configuration_values_handler {
 public:
  using container_entry = std::pair<configuration, configuration_values>;

  configuration_values_handler();
  virtual ~configuration_values_handler() = default;

  void add(const configuration& configuration_);
  void update(const configuration& configuration_);
  void remove(const wolf::types::uuid_array& id);
  using all_result = std::vector<configuration_values>;
  all_result get_all() const;
  std::optional<configuration_values> get(
      const wolf::types::uuid_array& id) const;
  configuration_values get_or_throw(const wolf::types::uuid_array& id) const;

  using outdoor_values = configuration_values_outdoor;
  const outdoor_values& get_last_outdoor_value() const;
  void set_outdoor_sensor(const wolf::outdoor_sensor& id);
  void reset_outdoor_humidity_value();

  void handle_values(const wolf::sensor_values& values);

  bool sensor_is_outdoor(const wolf::types::id_esp3& id) const;

  // TODO evaluate if these signals are still needed?
  // wolf::value_handler "kinda" has the same signals.
  boost::signals2::signal<void(const configuration_values&)> signal_value;
  boost::signals2::signal<void(const outdoor_values& value)>
      signal_values_outdoor;

 private:
  std::vector<configuration_values> handle_indoor(
      const wolf::sensor_values& values);
  bool handle_outdoor(const wolf::sensor_values& values);
  using container = std::vector<container_entry>;
  container::iterator find(const wolf::types::uuid_array& id);
  container::const_iterator find(const wolf::types::uuid_array& id) const;

 private:
  mutable logging::logger m_logger;
  container m_values;
  wolf::outdoor_sensor m_outdoor_sensor;
  outdoor_values m_last_outdoor_values;
};
}  // namespace mold

#endif  // MOLD_CONFIGURATION_VALUES_HANDLER_HPP
