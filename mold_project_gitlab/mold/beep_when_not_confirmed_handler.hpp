#ifndef MOLD_BEEP_WHEN_NOT_CONFIRMED_HANDLER_HPP
#define MOLD_BEEP_WHEN_NOT_CONFIRMED_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold/calculation_state.hpp"
#include "wolf/types.hpp"

namespace mold {

class beep_when_not_confirmed_handler {
 public:
  beep_when_not_confirmed_handler(boost::asio::io_service& service,
                                  const std::chrono::seconds& beep_interval);

  void handle_state(const wolf::types::uuid_array& config_id,
                    const calculation_state state);

  boost::signals2::signal<void()> signal_beep;
  boost::signals2::signal<void(wolf::types::uuid_array config_id)>
      signal_confirmed;

  void cancel();
  void confirm_all();

 private:
  void start_timer();
  void handle_beep(const boost::system::error_code& error);

 private:
  logging::logger m_logger;
  boost::asio::steady_timer m_beep_timer;
  const std::chrono::seconds m_beep_interval;

  std::vector<wolf::types::uuid_array> m_configs_that_are_not_confirmed;
};
}  // namespace mold

#endif  // MOLD_BEEP_WHEN_NOT_CONFIRMED_HANDLER_HPP
