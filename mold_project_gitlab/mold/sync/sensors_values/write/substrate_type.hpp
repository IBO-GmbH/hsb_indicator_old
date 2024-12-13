#ifndef MOLD_SYNC_SENSORS_VALUES_WRITE_SUBSTRATE_TYPE_HPP
#define MOLD_SYNC_SENSORS_VALUES_WRITE_SUBSTRATE_TYPE_HPP

#include "mold/configuration_handler.hpp"
#include "mold/options_handler.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "mold/sync/sensors_values/write/handler.hpp"

namespace mold::sync::sensors_values::write {

class substrate_type {
 public:
  substrate_type(handler& writer, options_handler& options,
                 configuration_handler& configurations,
                 room::settings::with_sensors::handler& room_handler);

 private:
  void connect_signals(room::settings::with_sensors::handler& room_handler);
  void handle_box_with_catch();
  void handle_box();
  void handle_configuration_with_catch(const configuration& configuration_);
  void handle_configuration(const configuration& configuration_);
  void send(const wolf::types::uuid_array& config_id,
            const mold::substrate_type& type);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  handler& m_writer;
  options_handler& m_options;
  configuration_handler& m_configurations;
};
}  // namespace mold::sync::sensors_values::write

#endif  // MOLD_SYNC_SENSORS_VALUES_WRITE_SUBSTRATE_TYPE_HPP
