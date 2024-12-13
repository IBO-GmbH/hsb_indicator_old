#ifndef MOLD_TONE_HANDLER_HPP
#define MOLD_TONE_HANDLER_HPP

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <thread>
#include "log/logger.hpp"

namespace wolf {
class tone_writer;
}

namespace mold {

class tone_handler : public boost::noncopyable {
 public:
  tone_handler(boost::asio::io_service& main_thread,
               const std::uint32_t& timeout, const std::uint32_t& pause,
               wolf::tone_writer& writer);
  ~tone_handler();

  void start(const bool more_than_once = false, const int max_beeps = 1,
             const int count = 1);
  void start_timer_pause(const int max_beeps, const int count);
  void cancel();

  void set_enabled(const bool enabled);
  void set_nightmode_active(const bool active);

 protected:
  void handle_timeout(const boost::system::error_code&,
                      const bool more_than_once, const int max_beeps = 1,
                      const int count = 0);

 private:
  logging::logger m_logger;
  boost::asio::io_service& m_main_thread;
  boost::asio::io_service m_service;
  boost::asio::io_service::work m_service_work;
  boost::asio::deadline_timer m_timer;
  std::thread m_service_thread;
  const std::uint32_t m_timeout;
  const std::uint32_t m_pause;
  wolf::tone_writer& m_writer;

  bool m_tone_active{false};
  bool m_enabled{true};
  bool m_nightmode_active{false};
};
}  // namespace mold

#endif  // MOLD_TONE_HANDLER_HPP
