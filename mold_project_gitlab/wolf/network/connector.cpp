#include "connector.hpp"

#include "wolf/url_helper.hpp"

using namespace wolf::network;

connector::connector(wolf::websocket_sync &websocket,
                     const connector::config &config)
    : m_logger{"network::connector"},
      m_websocket(websocket),
      m_config(config) {}

void connector::connect() {
  if (m_websocket.get_state() != websocket_sync::state::disconnected) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "already connecting or connected";
    return;
  }
  auto result_url =
      url_helper::combine_url_with_device_id(m_config.url, m_config.device_id);
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "connect, url:" << result_url;
  m_websocket.connect(result_url);
}
