#include "reconnect_handler.hpp"

using namespace mold;

reconnect_handler::reconnect_handler(boost::asio::io_service &service,
                                     wolf::websocket_sync &websocket,
                                     wolf::login_handler &login_handler,
                                     wolf::network::connector &connector,
                                     const wolf::config::network options)
    : m_logger{"reconnect_handler"},
      m_reconnect_timer{service},
      m_websocket(websocket),
      m_login_handler(login_handler),
      m_connector(connector),
      m_options{options},
      m_reconnect_timeout{options.reconnect_timeout} {
  auto reconnect = [this] { connect_after_timeout(); };
  m_slots.emplace_back(m_websocket.signal_failure.connect(reconnect));
  m_slots.emplace_back(m_websocket.signal_closed.connect(reconnect));
  m_slots.emplace_back(
      m_login_handler.signal_logged_in.connect([this](const bool logged_in) {
        // we are only interested in successful logins
        if (logged_in) reset_reconnect_timeout();
      }));
}

reconnect_handler::~reconnect_handler() { close(); }

void reconnect_handler::close() {
  m_close = true;
  cancel_timer();
}

void reconnect_handler::connect() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "doing a connect";
  if (m_close) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "connect cancelled due close is true";
    return;
  }
  cancel_timer();
  m_connector.connect();
}

void reconnect_handler::connect_after_timeout() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "connect_after_timeout, reconnect_timeout:" << std::dec
      << m_reconnect_timeout;
  if (m_close) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "cancelling reconnect, because of close!";
    return;
  }
  if (m_reconnect_timeout <= 0) {
    connect();
    return;
  }
  m_reconnect_timer.expires_from_now(std::chrono::seconds(m_reconnect_timeout));
  if (m_options.reconnect_timeout_double) {
    m_reconnect_timeout *= 2;
#ifdef NDEBUG
    m_reconnect_timeout = std::min(m_reconnect_timeout, 10 * 60);  // 10 minutes
#else
    m_reconnect_timeout = std::min(m_reconnect_timeout, 4);  // 4 seconds
#endif
  }
  m_reconnect_timer.async_wait([this](const boost::system::error_code &error) {
    handle_connect_timeout(error);
  });
}

void reconnect_handler::handle_connect_timeout(
    const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted) return;
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "handle_connect_timeout got called with an error:"
        << error.message();
    return;
  }
  connect();
}

void reconnect_handler::reset_reconnect_timeout() {
  m_reconnect_timeout = m_options.reconnect_timeout;
  connect();
}

void reconnect_handler::cancel_timer() {
  boost::system::error_code ignore_error;
  m_reconnect_timer.cancel(ignore_error);
}
