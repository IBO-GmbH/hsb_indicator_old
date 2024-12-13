#include "reset_reconnect_timer_on_signal.hpp"

using namespace mold;

reset_reconnect_timer_on_signal::reset_reconnect_timer_on_signal(
    boost::asio::io_context &context, reconnect_handler &reconnector)
    : m_logger{"reset_reconnect_timer_on_signal"},
      m_signals(context, SIGUSR1),
      m_reconnector(reconnector) {
  async_wait();
}

void reset_reconnect_timer_on_signal::async_wait() {
  m_signals.async_wait(
      [this](const auto &error, const auto) { handle(error); });
}

void reset_reconnect_timer_on_signal::handle(
    const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted) return;
  if (error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle got called with error:" << error.message();
    return;
  }
  LOGGING_LOG(m_logger, logging::severity::normal) << "handle";
  m_reconnector.reset_reconnect_timeout();
  async_wait();
}
