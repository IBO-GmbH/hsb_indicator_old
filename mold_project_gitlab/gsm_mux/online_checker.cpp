#include "online_checker.hpp"

using namespace logging;

online_checker::online_checker(boost::asio::io_service &service,
                               const std::string &url)
    : m_logger{"online_checker"},
      timer{service},
      resolver{service},
      socket{service},
      url(url) {
  ;
}

void online_checker::run() { start_timer(); }

void online_checker::stop() {
  boost::system::error_code error;
  timer.cancel(error);
}

void online_checker::start_timer() {
  LOGGING_LOG(m_logger, severity::normal) << " start_timer";
  timer.expires_from_now(std::chrono::minutes(10));
  timer.async_wait([this](const boost::system::error_code &error) {
    try {
      handle_timeout(error);
    } catch (const boost::system::error_code &error) {
      LOGGING_LOG(m_logger, severity::error)
          << "handle_timout, error:" << error.message();
    } catch (std::runtime_error &error) {
      LOGGING_LOG(m_logger, severity::error)
          << "handle_timout, error:" << error.what();
    }
  });
}

void online_checker::handle_timeout(const boost::system::error_code &error) {
  if (error == boost::asio::error::operation_aborted) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "handle_timeout: operation aborted!";
    return;
  }
  if (error) {
    throw error;
  }
  LOGGING_LOG(m_logger, severity::normal) << "starting to resolve";
  boost::asio::ip::tcp::resolver::query query(url, "http");
  resolver.async_resolve(
      query, [this](const boost::system::error_code &error,
                    const boost::asio::ip::tcp::resolver::iterator &resolved) {
        handle_resolve(error, resolved);
      });
}

void online_checker::handle_resolve(
    const boost::system::error_code &error,
    const boost::asio::ip::tcp::resolver::iterator &resolved) {
  LOGGING_LOG(m_logger, severity::normal)
      << "handle_resolve, error:" << error.message();
  if (check_for_error(error)) return;
  socket.async_connect(resolved->endpoint(),
                       [this](const boost::system::error_code &error) {
                         handle_connect(error);
                       });
}

void online_checker::handle_connect(const boost::system::error_code &error) {
  LOGGING_LOG(m_logger, severity::normal)
      << "::handle_connect, error:" << error.message();
  if (check_for_error(error)) return;
  boost::system::error_code error_close;
  socket.close(error_close);
  counter_failure = 0;
  start_timer();
}

bool online_checker::check_for_error(const boost::system::error_code &error) {
  if (!error) return false;
  if (error == boost::asio::error::operation_aborted) return false;

  LOGGING_LOG(m_logger, severity::error)
      << " check_for_error, error:" << error.message();
  handle_failure();
  return true;
}

void online_checker::handle_failure() {
  ++counter_failure;
  signal_offline(counter_failure);
}
