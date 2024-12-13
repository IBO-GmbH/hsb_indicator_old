#ifndef MOLD_SYNC_ROOM_SETTINGS_WITH_SENSORS_HANDLER_HPP
#define MOLD_SYNC_ROOM_SETTINGS_WITH_SENSORS_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/sync/room/cache.hpp"
#include "mold/sync/room/settings/with_sensors/sender.hpp"
#include "wolf/login_handler.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/room_handler.hpp"

namespace mold::sync::room::settings::with_sensors {

class handler {
 public:
  handler(cache& cache_, sender& sender_, wolf::room_handler& rooms,
          configuration_handler& configurations, wolf::login_handler& login,
          wolf::profile_handler& profile_handler);

  void start();
  void stop();
  void resync();
  void resync_all();

  boost::signals2::signal<void(const mold::configuration&)>
      signal_send_fake_values;

 private:
  void mark_dirty(const wolf::types::uuid_array& room_id);

  void connect_signals();
  void connect_signals_room();
  void connect_signals_configuration();
  void handle_room_modified(const wolf::room& room);
  void handle_configuration_modified(const configuration& configuration_);
  void mark_dirty_internal(const wolf::types::uuid_array& room_id);
  void handle_response(const std::optional<std::string>& error,
                       const wolf::types::uuid_array& room, const int version);
  void sync(const cache::dirty& dirty);
  void add_fake_sensors(
      const boost::optional<mold::configuration>& configuration,
      std::vector<wolf::sensor_id>& to_add_to);
  std::vector<wolf::sensor_profile> convert_ids_to_profiles(
      const std::vector<wolf::sensor_id>& ids);

  mold_settings get_mold_settings_from_configuration(
      const boost::optional<configuration>& from);

  logging::logger m_logger;
  cache& m_cache;
  sender& m_sender;
  wolf::room_handler& m_rooms;
  configuration_handler& m_configurations;
  wolf::login_handler& m_login;
  wolf::profile_handler& m_profile_handler;
  std::vector<boost::signals2::scoped_connection>
      m_signal_connections_rooms_configurations;
};

}  // namespace mold::sync::room::settings::with_sensors

#endif
