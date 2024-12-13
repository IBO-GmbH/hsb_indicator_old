#ifndef WOLF_WEBSOCKET_SYNC_HPP
#define WOLF_WEBSOCKET_SYNC_HPP

#include <boost/signals2/signal.hpp>
#include <websocketpp/client.hpp>
#include "websocket_config.hpp"

namespace wolf {

class websocket_sync {
 public:
  using client = websocketpp::client<websocket_config>;
  using package = std::vector<std::uint8_t>;

  enum struct state { disconnected, connecting, connected };

  struct websocket_values {
    const int &ping_timeout;
    const int &pong_timeout;
    const bool &gzip_compression;
  };

  websocket_sync(boost::asio::io_service &service,
                 const websocket_values &websocket_values);
  virtual ~websocket_sync() = default;
  bool connect(const std::string &url);
  void close();

  virtual bool send(const package &to_send);

  state get_state() const;

  boost::signals2::signal<void()> signal_connected;
  boost::signals2::signal<void()> signal_failure;
  boost::signals2::signal<void()> signal_closed;
  boost::signals2::signal<void(std::string)> signal_message;
  boost::signals2::signal<void()> signal_pong_timeout;

 private:
  void handle_connected(websocketpp::connection_hdl hdl);
  using message = websocketpp::config::asio_client::message_type::ptr;
  void handle_message(websocketpp::connection_hdl hdl, message message);
  void handle_closed(websocketpp::connection_hdl hdl);
  void handle_fail(websocketpp::connection_hdl hdl);
  bool handle_ping(websocketpp::connection_hdl hdl, const std::string &message);
  void handle_pong(websocketpp::connection_hdl hdl);
  void handle_pong_timeout(websocketpp::connection_hdl hdl,
                           const std::string &payload);

  void start_send_ping();
  void handle_send_ping(const boost::system::error_code &error);

 private:
  logging::logger m_logger;

  client m_client;
  websocketpp::connection_hdl m_connection_handle;

  boost::asio::deadline_timer m_ping;
  int m_ping_timeout;
  state m_state{state::disconnected};
  bool m_gzip_compression;
};
}  // namespace wolf

#endif  // WOLF_WEBSOCKET_SYNC_HPP
