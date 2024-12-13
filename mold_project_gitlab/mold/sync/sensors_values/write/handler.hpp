#ifndef MOLD_SYNC_SENSORS_VALUES_WRITE_HANDLER_HPP
#define MOLD_SYNC_SENSORS_VALUES_WRITE_HANDLER_HPP

#include "mold/sync/sensors_values/write/cache.hpp"
#include "mold/sync/sensors_values/write/sender.hpp"
#include "wolf/login_handler.hpp"
#include "wolf/outdoor_sensor.hpp"
#include "wolf/room.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace mold::sync::sensors_values::write {

class handler {
 public:
  handler(cache& cache_, sender& sender_,
          wolf::sensor_value_handler& sensor_values,
          wolf::login_handler& login);
  void resync();
  void handle(const wolf::sensor_values& values);
  void reset_values_to_send();
  using sensors = std::vector<wolf::sensor_id>;
  void handle_sensors_to_remove(const sensors& sensors_,
                                const std::vector<wolf::room>& all_rooms,
                                const wolf::outdoor_sensor& outdoor_sensor);

 private:
  void connect_signals();
  void resync_some();
  void handle_resync_response(const std::optional<std::string>& error,
                              const std::vector<cache::key>& keys);
  void handle_response(const std::optional<std::string>& error,
                       const wolf::sensor_values& values);
  void handle_offline(const wolf::sensor_values& values);
  void handle_online(const wolf::sensor_values& values);
  void remove_all_values_for_sensors(const sensors& sensors_);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  cache& m_cache;
  sender& m_sender;
  wolf::sensor_value_handler& m_sensor_values;
  wolf::login_handler& m_login;
};

}  // namespace mold::sync::sensors_values::write

#endif
