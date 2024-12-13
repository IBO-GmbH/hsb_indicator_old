#include "handler.hpp"

using namespace mold::sync::room::settings;

handler::handler(cache &cache_, parser &parser_,
                 with_sensors::handler &sync_sender, wolf::room_handler &rooms,
                 mold::configuration_handler &configurations)
    : m_logger{"sync::room::settings::handler"},
      m_cache(cache_),
      m_parser(parser_),
      m_sync_sender(sync_sender),
      m_rooms(rooms),
      m_configurations(configurations) {
  connect_signals();
}

void handler::connect_signals() {
  m_signal_connections.emplace_back(m_parser.signal_parsed.connect(
      [this](auto response, const auto information) {
        handle_message_with_catch(response, information);
      }));
}

void handler::handle_message_with_catch(
    wolf::network::response::send_helper &response,
    const parser::information &information) {
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
  with_sensors::handler &to_stop;
  sync_send_stopper(with_sensors::handler &to_stop) : to_stop(to_stop) {
    to_stop.stop();
  }
  ~sync_send_stopper() { to_stop.start(); }
};
}  // namespace

void handler::handle_message(wolf::network::response::send_helper &response,
                             const parser::information &information) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "handle_message, information:" << information;
  sync_send_stopper stopper{m_sync_sender};
  const auto &room_id = information.room;
  {
    auto room = m_rooms.get(room_id);
    room.name = information.name;
    m_rooms.update(room);
  }
  m_cache.raise_version(room_id);
  auto configuration = m_configurations.get_by_room_id_or_throw(room_id);
  configuration.mold_settings_ = boost::optional<mold_settings>();
  mold_settings settings{building_type::none, 0.f, substrate_type::none};
  if (information.building_type_ != building_type::none) {
    settings.building_type_ = information.building_type_;
    settings.frsi = information.threshold_factor;
  }
  if (information.substrate_type_ != substrate_type::none)
    settings.substrate_type_ = information.substrate_type_;
  if (settings.building_type_ != building_type::none ||
      settings.substrate_type_ != substrate_type::none)
    configuration.mold_settings_ = settings;
  m_configurations.update(configuration);
  response.success();
}
