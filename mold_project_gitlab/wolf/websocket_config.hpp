#ifndef WOLF_WEBSOCKET_CONFIG_HPP
#define WOLF_WEBSOCKET_CONFIG_HPP

#include <websocketpp/config/asio_client.hpp>
#include "websocket_logger.hpp"

namespace wolf {

struct websocket_config : public websocketpp::config::asio_tls_client {
  websocket_config() = delete;

  using type = websocket_config;
  using alog_type = websocket_logger<concurrency_type, websocketpp::log::alevel,
                                     logging::severity::verbose>;
  using elog_type = websocket_logger<concurrency_type, websocketpp::log::alevel,
                                     logging::severity::warning>;
  struct transport_config
      : public websocketpp::config::asio_tls_client::transport_config {
    using alog_type = type::alog_type;
    using elog_type = type::elog_type;
  };
  using transport_type =
      websocketpp::transport::asio::endpoint<transport_config>;

  static const long timeout_socket_pre_init = 20000;
  static const long timeout_proxy = 20000;
  static const long timeout_socket_post_init = 20000;
  static const long timeout_dns_resolve = 20000;
  static const long timeout_connect = 20000;
  static const long timeout_socket_shutdown = 5000;

#define WEBSOCKET_LOG_ALL 0
#if WEBSOCKET_LOG_ALL
  static const websocketpp::log::level alog_level =
      websocketpp::log::alevel::all /* ^ websocketpp::log::alevel::devel*/;
  static const websocketpp::log::level elog_level =
      websocketpp::log::elevel::all /* ^ websocketpp::log::alevel::devel*/;
#else
  static const websocketpp::log::level alog_level =
      websocketpp::log::alevel::all ^ websocketpp::log::alevel::devel ^
      websocketpp::log::alevel::frame_payload ^
      websocketpp::log::alevel::frame_header;
  static const websocketpp::log::level elog_level =
      websocketpp::log::elevel::all ^ websocketpp::log::elevel::devel ^
      websocketpp::log::alevel::frame_payload ^
      websocketpp::log::alevel::frame_header;
#endif
#undef WEBSOCKET_LOG_ALL
};
}  // namespace wolf

#endif  // WOLF_WEBSOCKET_CONFIG_HPP
