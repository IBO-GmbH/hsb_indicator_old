#include "tone_handler.hpp"

#include "wolf/tone_writer.hpp"

using namespace logging;
using namespace mold;

tone_handler::tone_handler(boost::asio::io_service &main_thread,
                           const uint32_t &timeout, const uint32_t &pause,
                           wolf::tone_writer &writer)
    : m_logger{"tone_handler"},
      m_main_thread(main_thread),
      m_service_work(m_service),
      m_timer(m_service),
      m_service_thread([this] { m_service.run(); }),
      m_timeout{timeout},
      m_pause{pause},
      m_writer(writer) {
  LOGGING_LOG(m_logger, severity::normal)
      << "started with timeout:" << std::dec << timeout
      << "ms and pause:" << std::dec << pause << "ms";
}

tone_handler::~tone_handler() {
  m_service.stop();
  if (m_service_thread.joinable()) m_service_thread.join();
}

void tone_handler::start(const bool more_than_once, const int max_beeps,
                         const int count) {
  if (m_nightmode_active || m_tone_active || !m_enabled) return;
  m_tone_active = true;
  m_service.post([this] { m_writer.set_enabled(true); });
  m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout));
  m_timer.async_wait([this, more_than_once, max_beeps,
                      count](const boost::system::error_code &error) {
    handle_timeout(error, more_than_once, max_beeps, count);
  });
}

void tone_handler::start_timer_pause(const int max_beeps, const int count) {
  m_timer.expires_from_now(boost::posix_time::milliseconds(m_pause));
  m_timer.async_wait(
      [this, max_beeps, count](const boost::system::error_code &) {
        start(true, max_beeps, (count + 1));
      });
}

void tone_handler::cancel() {
  boost::system::error_code error;
  m_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel timer, message:" << error.message();
  m_service.post([this] { m_writer.set_enabled(false); });
}

void tone_handler::set_enabled(const bool enabled) {
  LOGGING_LOG(m_logger, severity::verbose)
      << (enabled ? "enabled" : "disbaled");
  m_enabled = enabled;
  cancel();
}

void tone_handler::set_nightmode_active(const bool active) {
  if (m_nightmode_active == active) return;
  m_nightmode_active = active;
}

void tone_handler::handle_timeout(const boost::system::error_code &,
                                  const bool more_than_once,
                                  const int max_beeps, const int count) {
  m_writer.set_enabled(false);
  m_main_thread.post([this] { m_tone_active = false; });
  if (!more_than_once || count == max_beeps) return;
  start_timer_pause(max_beeps, count);
}
