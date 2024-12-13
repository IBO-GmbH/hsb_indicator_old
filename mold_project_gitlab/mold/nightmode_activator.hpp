#ifndef MOLD_NIGHTMODE_ACTIVATOR_HPP
#define MOLD_NIGHTMODE_ACTIVATOR_HPP

#include "nightmode_handler.hpp"

namespace mold {

class nightmode_activator {
 public:
  using time_point = std::chrono::system_clock::time_point;

  nightmode_activator(const mold::config& program_options,
                      nightmode_handler& nightmode,
                      boost::asio::io_service& service);
  void activate(const time_point& time_to_check);
  void deactivate();

 private:
  void check_if_enabled();
  void check(const time_point& time_to_check);
  void handle_activate(const time_point& time_to_check);
  std::time_t calculate_time(const time_point& time_to_check) const;
  void handle_wait(const boost::system::error_code& error);

 private:
  logging::logger m_logger;
  const mold::config& m_program_options;
  nightmode_handler& m_nightmode;
  boost::asio::deadline_timer m_timer;
};
}  // namespace mold

#endif  // MOLD_NIGHTMODE_ACTIVATOR_HPP
