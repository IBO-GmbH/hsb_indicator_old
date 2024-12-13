#ifndef MOLD_CONNECTION_HANDLER_HPP
#define MOLD_CONNECTION_HANDLER_HPP

#include <boost/asio/steady_timer.hpp>
#include "wolf/login_handler.hpp"
#include "wolf/network/connector.hpp"
#include "wolf/program_options_parser.hpp"
#include "wolf/websocket_sync.hpp"

namespace mold {

class reconnect_handler {
 public:
  reconnect_handler(boost::asio::io_service& service,
                    wolf::websocket_sync& websocket,
                    wolf::login_handler& login_handler,
                    wolf::network::connector& connector,
                    const wolf::config::network options);
  ~reconnect_handler();
  void connect();
  void close();
  void reset_reconnect_timeout();

 private:
  void connect_after_timeout();
  void handle_connect_timeout(const boost::system::error_code& error);
  void cancel_timer();

  logging::logger m_logger;
  boost::asio::steady_timer m_reconnect_timer;
  wolf::websocket_sync& m_websocket;
  wolf::login_handler& m_login_handler;
  wolf::network::connector& m_connector;
  const wolf::config::network m_options;
  int m_reconnect_timeout;
  bool m_close{};
  std::vector<boost::signals2::scoped_connection> m_slots;
};

}  // namespace mold

#endif  // MOLD_CONNECTION_HANDLER_HPP
