#include "handler.hpp"
#include "mold/sync/sensor_id_frsi.hpp"
#include "mold/sync/sensor_id_mold_state.hpp"
#include "mold/sync/sensor_id_substrate.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace mold::sync::room::settings::with_sensors;

handler::handler(cache &cache_, sender &sender_, wolf::room_handler &rooms,
                 configuration_handler &configurations,
                 wolf::login_handler &login,
                 wolf::profile_handler &profile_handler)
    : m_logger("sync::room::settings::with_sensors::handler"),
      m_cache(cache_),
      m_sender(sender_),
      m_rooms(rooms),
      m_configurations(configurations),
      m_login(login),
      m_profile_handler(profile_handler) {
  connect_signals();
}

void handler::start() {
  stop();
  connect_signals_room();
  connect_signals_configuration();
}

void handler::stop() { m_signal_connections_rooms_configurations.clear(); }

void handler::resync() {
  try {
    if (!m_login.is_logged_in()) return;
    const auto to_sync = m_cache.get_dirty_but_not_deleted();
    for (const auto &sync_ : to_sync) sync(sync_);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "fail to to 'resync', what():" << error.what();
  }
}

void handler::resync_all() {
  LOGGING_LOG(m_logger, logging::severity::normal) << "resync_all";
  const auto all = m_rooms.get_all();
  for (const auto &room : all) mark_dirty(room.id);
}

void handler::mark_dirty(const wolf::types::uuid_array &room_id) {
  try {
    mark_dirty_internal(room_id);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to to 'mark_dirty', what():" << error.what();
  }
}

void handler::connect_signals() { start(); }

void handler::connect_signals_room() {
  const auto callback_room_modified = [this](const wolf::room &room) {
    handle_room_modified(room);
  };
  m_signal_connections_rooms_configurations.emplace_back(
      m_rooms.signal_updated.connect(callback_room_modified));
}

void handler::connect_signals_configuration() {
  const auto callback_configuration_modified =
      [this](const configuration &configuration_) {
        handle_configuration_modified(configuration_);
      };
  m_signal_connections_rooms_configurations.emplace_back(
      m_configurations.signal_added.connect(callback_configuration_modified));
  m_signal_connections_rooms_configurations.emplace_back(
      m_configurations.signal_updated.connect(callback_configuration_modified));
}

void handler::handle_room_modified(const wolf::room &room) {
  mark_dirty(room.id);
}

void handler::handle_configuration_modified(
    const mold::configuration &configuration_) {
  mark_dirty(configuration_.room);
}

void handler::mark_dirty_internal(const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "mark_dirty, room_id:" << room_id;
  if (m_cache.is_marked_or_actually_deleted(room_id)) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "tried to delete a room which is in the 'deleted' or actually "
           "deleted, room_id:"
        << room_id;
    return;
  }
  m_cache.mark_dirty(room_id);
  resync();
}

void handler::handle_response(const std::optional<std::string> &error,
                              const wolf::types::uuid_array &room,
                              const int version) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_response_dirty, room:" << room << ", version:" << version;
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "handle_response_dirty failed, error:" << error.value();
    return;
  }
  try {
    m_cache.synchronized(room, version);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "fail in 'handle_response_dirty', what():" << error.what();
  }
}

void handler::add_fake_sensors(
    const boost::optional<mold::configuration> &configuration,
    std::vector<wolf::sensor_id> &to_add_to) {
  if (!configuration) return;
  const auto &config_id = configuration->id;
  to_add_to.push_back(mold::sync::sensor_id_frsi::create(config_id));
  to_add_to.push_back(mold::sync::sensor_id_substrate::create(config_id));
  if (configuration.get().temperature.is_set() &&
      configuration.get().humidity.is_set() &&
      m_configurations.get_outdoor_sensor().temperature.is_set())
    to_add_to.push_back(mold::sync::sensor_id_mold_state::create(config_id));
}

std::vector<wolf::sensor_profile> handler::convert_ids_to_profiles(
    const std::vector<wolf::sensor_id> &ids) {
  std::vector<wolf::sensor_profile> profiles;
  for (const auto &sensor_id : ids) {
    if (wolf::sensor_value_type_helper::is_electric_energy(sensor_id)) continue;
    auto profile = m_profile_handler.get(sensor_id);
    profiles.push_back(profile);
  }
  return profiles;
}

mold::mold_settings handler::get_mold_settings_from_configuration(
    const boost::optional<mold::configuration> &from) {
  if (!from) return {mold::building_type::none, 0.f, substrate_type::none};
  const auto settings = from.value().mold_settings_;
  if (!settings) return {mold::building_type::none, 0.f, substrate_type::none};
  const auto settings_value = settings.value();
  return {settings_value.building_type_, settings_value.frsi,
          settings_value.substrate_type_};
}

void handler::sync(const cache::dirty &dirty) {
  const auto room_id = dirty.room_id;
  const wolf::room room = m_rooms.get(room_id);
  const auto configuration_ = m_configurations.get_by_room_id(room_id);
  const auto settings = get_mold_settings_from_configuration(configuration_);
  auto sensors = room.sensors;
  add_fake_sensors(configuration_, sensors);
  const auto profiles = convert_ids_to_profiles(sensors);
  const sender::information to_send{room_id,
                                    room.name,
                                    profiles,
                                    settings.frsi,
                                    settings.building_type_,
                                    settings.substrate_type_};
  m_sender.send(to_send,
                [this, room_id, version = dirty.version](const auto &error) {
                  handle_response(error, room_id, version);
                });
  if (configuration_.has_value())
    signal_send_fake_values(configuration_.value());
}
