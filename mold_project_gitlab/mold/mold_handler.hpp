#ifndef MOLD_MOLD_HANDLER_HPP
#define MOLD_MOLD_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/configuration_state_handler.hpp"
#include "mold/configuration_values_handler.hpp"
#include "mold/configuration_values_outdoor.hpp"
#include "mold/evaluation_humidity/handler.hpp"
#include "mold/mold_value_handler.hpp"
#include "mold/options_handler.hpp"
#include "mold/sprout/handler.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold {

class mold_handler {
 public:
  mold_handler(configuration_handler& handler,
               configuration_values_handler& value,
               configuration_state_handler& state,
               evaluation_humidity::handler& evaluation_humidity,
               sprout::handler& sprout,
               wolf::sensor_value_handler& value_handler,
               wolf::room_handler& room_handler,
               mold_value_handler& mold_value_, options_handler& options);

 private:
  void setup_value_handler();
  void setup_handler();
  void setup_configuration_calculator();
  void setup_mold_value();
  void setup_configuration_values();
  void setup_options_handler();
  void handle_configuration_added(const mold::configuration& added);
  void handle_configuration_updated(const mold::configuration& updated);
  void handle_configuration_removed(const wolf::types::uuid_array& id);
  void handle_evaluation_humidity_median(
      const wolf::types::uuid_array& config_id,
      const wolf::sensor_value& humidity);
  void handle_evaluation_humidity_filtered(const wolf::sensor_value& humidity);
  void handle_mold_value(const mold_value& value);
  void handle_values(const wolf::sensor_values& values);
  void handle_handler_outdoor_changed(const wolf::outdoor_sensor& old,
                                      const wolf::outdoor_sensor& new_);
  void handle_configuration_values(const mold::configuration_values& values);
  void handle_configuration_outdoor_values(
      const mold::configuration_values_outdoor& outdoor);
  void handle_substrate_changed_all();
  void handle_substrate_changed_one(const mold::configuration& config);

  logging::logger m_logger;
  configuration_handler& m_handler;
  configuration_values_handler& m_value;
  configuration_state_handler& m_state;
  evaluation_humidity::handler& m_evaluation_humidity;
  sprout::handler& m_sprout;
  wolf::sensor_value_handler& m_value_handler;
  wolf::room_handler& m_rooms;
  mold_value_handler& m_mold_value;
  options_handler& m_options;
};

}  // namespace mold

#endif  // MOLD_HANDLER_HPP
