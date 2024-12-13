#include "websocket_sync.hpp"

#include "types.hpp"

using namespace logging;
using namespace wolf;

websocket_sync::websocket_sync(
    boost::asio::io_service &service,
    const wolf::websocket_sync::websocket_values &websocket_values)
    : m_logger{"websocket_sync"},
      m_ping(service),
      m_ping_timeout{websocket_values.ping_timeout},
      m_gzip_compression{websocket_values.gzip_compression} {
  m_client.init_asio(&service);
  m_client.set_open_handler(std::bind(&websocket_sync::handle_connected, this,
                                      std::placeholders::_1));
  m_client.set_message_handler(std::bind(&websocket_sync::handle_message, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  m_client.set_close_handler(
      std::bind(&websocket_sync::handle_closed, this, std::placeholders::_1));
  m_client.set_fail_handler(
      std::bind(&websocket_sync::handle_fail, this, std::placeholders::_1));
  m_client.set_ping_handler(std::bind(&websocket_sync::handle_ping, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
  m_client.set_pong_handler(
      std::bind(&websocket_sync::handle_pong, this, std::placeholders::_1));
  m_client.set_tls_init_handler([this](websocketpp::connection_hdl) {
    auto result = websocketpp::lib::make_shared<boost::asio::ssl::context>(
        boost::asio::ssl::context::tlsv12);
    boost::system::error_code error;
    result->set_options(boost::asio::ssl::context::default_workarounds |
                            boost::asio::ssl::context::no_sslv2 |
                            boost::asio::ssl::context::no_sslv3 |
                            boost::asio::ssl::context::single_dh_use,
                        error);
    if (error) {
      LOGGING_LOG(m_logger, severity::error)
          << "could not set tls context options, message:" << error.message();
    }
    return result;
  });
  if (websocket_values.pong_timeout > 0) {
    m_client.set_pong_timeout_handler(
        std::bind(&websocket_sync::handle_pong_timeout, this,
                  std::placeholders::_1, std::placeholders::_2));
    m_client.set_pong_timeout(
        websocket_values.pong_timeout *
        1000);  // *1000 for converting seconds to milliseconds
  }
}

bool websocket_sync::connect(const std::string &url) {
  LOGGING_LOG(m_logger, severity::normal) << "connecting to " << url;

  websocketpp::lib::error_code error;
  client::connection_ptr connection = m_client.get_connection(url, error);
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "websocket_sync::connect failed with error:" << error.message();
    return false;
  }
  m_client.connect(connection);
  m_state = state::connecting;
  return true;
}

void websocket_sync::close() {
  LOGGING_LOG(m_logger, severity::verbose) << "closing connection";

  websocketpp::lib::error_code error;
  m_client.close(m_connection_handle, websocketpp::close::status::normal,
                 "shutdown", error);
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not close connection, error:" << error.message();
  }
}

bool websocket_sync::send(const websocket_sync::package &to_send) {
  websocketpp::lib::error_code error;
  const websocketpp::frame::opcode::value opcode{
      websocketpp::frame::opcode::binary};
  m_client.send(m_connection_handle, to_send.data(), to_send.size(), opcode,
                error);
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "an error occured when trying to send, message:" << error.message();
    return false;
  }
  return true;
}

websocket_sync::state websocket_sync::get_state() const { return m_state; }

void websocket_sync::handle_connected(websocketpp::connection_hdl hdl) {
  LOGGING_LOG(m_logger, severity::normal)
      << "successfully connected with websocket";
  m_connection_handle = hdl;
  m_state = state::connected;
  // start ping
  start_send_ping();

  signal_connected();
}

void websocket_sync::handle_message(websocketpp::connection_hdl /*hdl*/,
                                    websocket_sync::message message) {
  LOGGING_LOG(m_logger, severity::normal)
      << "received a websocket message,"
         " payload.size():"
      << std::dec << message->get_payload().size()
      << " extension_data.size():" << message->get_extension_data().size()
      << " compressed:" << std::boolalpha << message->get_compressed();
  signal_message(message->get_payload());
}

void websocket_sync::handle_closed(websocketpp::connection_hdl /*hdl*/) {
  LOGGING_LOG(m_logger, severity::normal) << "websocket closed";

  boost::system::error_code error;
  if (m_ping_timeout > 0) {
    m_ping.cancel(error);
    if (error) {
      LOGGING_LOG(m_logger, severity::warning)
          << "could not cancel websocket ping, message:" << error.message();
    }
  }
  m_state = state::disconnected;

  signal_closed();
}

void websocket_sync::handle_fail(websocketpp::connection_hdl /*hdl*/) {
  LOGGING_LOG(m_logger, severity::warning) << "websocket failed";
  m_state = state::disconnected;
  signal_failure();
}

bool websocket_sync::handle_ping(websocketpp::connection_hdl /*hdl*/,
                                 const std::string &message) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "websocket_sync::handle_ping, message.size():" << std::dec
      << message.size();
  return true;
}

void websocket_sync::handle_pong(websocketpp::connection_hdl /*hdl*/) {
  LOGGING_LOG(m_logger, severity::verbose) << "websocket_sync::handle_pong";
}

void websocket_sync::handle_pong_timeout(websocketpp::connection_hdl /*hdl*/,
                                         const std::string &payload) {
  LOGGING_LOG(m_logger, severity::error)
      << "a pong timeout occured, payload:" << payload;
  signal_pong_timeout();
}

void websocket_sync::start_send_ping() {
  if (m_ping_timeout <= 0) {
    return;
  }
  m_ping.expires_from_now(boost::posix_time::seconds(m_ping_timeout));
  m_ping.async_wait(std::bind(&websocket_sync::handle_send_ping, this,
                              std::placeholders::_1));
}

void websocket_sync::handle_send_ping(const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::error)
        << "handle_send_ping got called with the error-message:"
        << error.message();
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose) << "sending ping";
  websocketpp::lib::error_code error_websocket;
  m_client.ping(m_connection_handle, std::string(), error_websocket);
  if (error_websocket) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to send a ping, message:" << error_websocket.message();
    return;
  }

  start_send_ping();
}
