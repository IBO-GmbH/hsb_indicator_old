#ifndef WOLF_NETWORK_CONNECTOR_HPP
#define WOLF_NETWORK_CONNECTOR_HPP

#include "wolf/websocket_sync.hpp"

namespace wolf::network {

class connector {
 public:
  struct config {
    std::string url;
    std::uint64_t device_id;
  };

  connector(websocket_sync& websocket, const config& config);
  void connect();

 private:
  logging::logger m_logger;
  websocket_sync& m_websocket;
  const config m_config;
};

}  // namespace wolf::network

#endif
