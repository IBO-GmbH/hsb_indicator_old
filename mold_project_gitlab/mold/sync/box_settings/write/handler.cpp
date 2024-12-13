#include "handler.hpp"

using namespace mold::sync::box_settings::write;

handler::handler(mold::sync::box_settings::cache &cache_, sender &sender_,
                 wolf::login_handler &login_,
                 mold::configuration_handler &configurations,
                 mold::options_handler &options)
    : m_logger{"sync::box_settings::write::handler"},
      m_cache(cache_),
      m_sender(sender_),
      m_login(login_),
      m_configurations(configurations),
      m_options(options) {
  connect_signals();
}

void handler::start() {
  stop();
  connect_signals();
}

void handler::stop() { m_signal_connections.clear(); }

void handler::resync() {
  try {
    resync_internal();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "fail to to 'resync', what():" << error.what();
  }
}

void handler::resync_all() { mark_dirty(); }

void handler::connect_signals() {
  m_signal_connections.emplace_back(
      m_configurations.signal_outdoor_changed.connect(
          [this](const auto &, const auto &) { mark_dirty_with_catch(); }));
  m_signal_connections.emplace_back(m_options.signal_frsi_updated.connect(
      [this] { mark_dirty_with_catch(); }));
  m_signal_connections.emplace_back(
      m_options.signal_substrate_type_changed.connect(
          [this] { mark_dirty_with_catch(); }));
}

void handler::mark_dirty_with_catch() {
  try {
    mark_dirty();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to to 'mark_dirty', what():" << error.what();
  }
}

void handler::mark_dirty() {
  m_cache.mark_dirty();
  resync();
}

void handler::resync_internal() {
  if (!m_login.is_logged_in()) return;
  const auto dirty = m_cache.get();
  if (!dirty.is_dirty) return;
  const auto building_type = m_options.get_building_type();
  const auto threshold_factor = m_options.get_frsi();
  const auto outdoor_sensor = m_configurations.get_outdoor_sensor();
  const auto substrate_type = m_options.get_substrate_type();
  const entity to_send{outdoor_sensor, building_type, threshold_factor,
                       substrate_type};
  m_sender.send(to_send, [this, version = dirty.version](const auto &error) {
    handle_response(error, version);
  });
}

void handler::handle_response(const std::optional<std::string> &error,
                              const int version) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle_response failed, version:" << std::dec << version;
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "handle_response failed, error:" << error.value();
    return;
  }
  try {
    m_cache.synchronized(version);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "fail in 'handle_response', what():" << error.what();
  }
}
