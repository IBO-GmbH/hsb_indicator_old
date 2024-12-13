#ifndef MOLD_DISPLAY_HANDLER_HPP
#define MOLD_DISPLAY_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold/configuration_state_handler.hpp"
#include "mold/display_handler_base.hpp"
#include "mold/program_options_parser.hpp"

namespace mold {

class display_handler : public display_handler_base {
 public:
  display_handler(const mold::config& program_options,
                  boost::asio::io_service& io_service,
                  mold::configuration_state_handler& state_handler);
  void set_on(const bool on) override;
  bool display_is_locked() const override;
  void enable_states_update(const bool enable) override;
  void handle_states_update() override;
  void handle_co2_state_update(const calculation_state& state);
  virtual ~display_handler() override = default;

  boost::signals2::signal<void(const bool)> signal_on;

 private:
  void set_lock_power_off(const bool disabled);
  void start_timer();
  void display_on();
  void display_off();
  void execute_script(const std::string& script);
  void change_on(const bool on);
  void change_display_locked(const bool display_locked);
  void handle_wait(const boost::system::error_code& error);
  bool all_green_or_confirmed();
  static bool green_or_confirmed(const calculation_state& state);

 private:
  logging::logger m_logger;

  const mold::config& m_config;
  mold::configuration_state_handler& m_state_handler;

  bool m_on{false};
  bool m_display_locked{false};
  bool m_states_update_enabled{true};

  boost::asio::steady_timer m_timer;
};
}  // namespace mold

#endif  // MOLD_DISPLAY_HANDLER_HPP
