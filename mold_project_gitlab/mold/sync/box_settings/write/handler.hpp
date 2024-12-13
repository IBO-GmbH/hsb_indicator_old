#ifndef MOLD_SYNC_BOX_SETTINGS_WRITE_HANDLER_HPP
#define MOLD_SYNC_BOX_SETTINGS_WRITE_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/options_handler.hpp"
#include "mold/sync/box_settings/cache.hpp"
#include "mold/sync/box_settings/write/sender.hpp"
#include "wolf/login_handler.hpp"

namespace mold::sync::box_settings::write {
class handler {
 public:
  handler(cache& cache_, sender& sender_, wolf::login_handler& login_,
          configuration_handler& configurations, options_handler& options);

  void start();
  void stop();
  void resync();
  void resync_all();

 private:
  void connect_signals();
  void mark_dirty_with_catch();
  void mark_dirty();
  void resync_internal();
  void handle_response(const std::optional<std::string>& error,
                       const int version);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  cache& m_cache;
  sender& m_sender;
  wolf::login_handler& m_login;
  configuration_handler& m_configurations;
  options_handler& m_options;
};
}  // namespace mold::sync::box_settings::write
#endif
