#ifndef MOLD_SYNC_SENSORS_VALUES_WRITE_MOLD_STATE_HPP
#define MOLD_SYNC_SENSORS_VALUES_WRITE_MOLD_STATE_HPP

#include "mold/configuration_state_handler.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "mold/sync/sensors_values/write/handler.hpp"

namespace mold::sync::sensors_values::write {

class mold_state {
 public:
  mold_state(handler& writer, configuration_state_handler& state_handler,
             outdoor_sensor_change_helper& outdoor_helper,
             room::settings::with_sensors::handler& room_handler);

 private:
  void connect_signals(room::settings::with_sensors::handler& room_handler);
  void handle_with_catch(const wolf::types::uuid_array& id,
                         const calculation_state& state);
  void handle(const wolf::types::uuid_array& id,
              const calculation_state& state);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  handler& m_writer;
  configuration_state_handler& m_state_handler;
  outdoor_sensor_change_helper& m_outdoor_helper;
};

}  // namespace mold::sync::sensors_values::write

#endif
