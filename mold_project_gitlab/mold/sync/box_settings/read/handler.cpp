#include "handler.hpp"

using namespace mold::sync::box_settings::read;
using namespace mold::sync::box_settings;
using namespace mold::sync;

handler::handler(parser &parser_, cache &cache_, write::handler &writer,
                 mold::options_handler &options,
                 wolf::profile_handler &profiles,
                 mold::outdoor_sensor_change_helper &outdoor_sensor_setter)
    : m_logger{"sync::box_settings::read::handler"},
      m_parser(parser_),
      m_cache(cache_),
      m_writer(writer),
      m_options(options),
      m_profiles(profiles),
      m_outdoor_sensor_setter(outdoor_sensor_setter) {
  connect_signals();
}

void handler::connect_signals() {
  m_signal_connections.emplace_back(m_parser.signal_parsed.connect(
      [this](auto &response, const auto &information) {
        handle_message_with_catch(response, information);
      }));
}

void handler::handle_message_with_catch(
    wolf::network::response::send_helper &response, const entity &information) {
  try {
    handle_message(response, information);
  } catch (const std::runtime_error &error) {
    try {
      std::stringstream description;
      description << "could not handle message, error:" << error.what();
      const auto description_string = description.str();
      LOGGING_LOG(m_logger, logging::severity::error) << description_string;
      response.failure(description_string);
    } catch (...) {
    }
  }
}

namespace {
// https://stackoverflow.com/questions/6379422/c-multiple-classes-with-same-name
struct sync_send_stopper {
  write::handler &to_stop;
  sync_send_stopper(write::handler &to_stop) : to_stop(to_stop) {
    to_stop.stop();
  }
  sync_send_stopper(const sync_send_stopper &) = delete;
  ~sync_send_stopper() { to_stop.start(); }
};
}  // namespace

void handler::handle_message(wolf::network::response::send_helper &response,
                             const entity &information) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "handle_message, information:" << information;
  const sync_send_stopper stopper{m_writer};
  const wolf::sensor_profile profile_temperature = {
      information.outdoor_sensor.temperature, wolf::sensor_type::foreign, {}};
  const wolf::sensor_profile profile_humidity{
      information.outdoor_sensor.humidity, wolf::sensor_type::foreign, {}};
  m_profiles.learn(profile_temperature);
  m_profiles.learn(profile_humidity);
  m_options.set_building_type_and_frsi(information.building_type_,
                                       information.threshold_factor);
  m_options.set_substrate_type(information.substrate_type_);
  m_outdoor_sensor_setter.set(information.outdoor_sensor);
  m_cache.raise_version();
  response.success();
}
