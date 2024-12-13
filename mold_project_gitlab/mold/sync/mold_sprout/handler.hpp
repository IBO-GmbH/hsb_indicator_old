#ifndef MOLD_SYNC_MOLD_SPROUT_HANDLER_HPP
#define MOLD_SYNC_MOLD_SPROUT_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/sprout/handler.hpp"
#include "mold/sync/mold_sprout/cache.hpp"
#include "mold/sync/mold_sprout/sender.hpp"
#include "wolf/login_handler.hpp"

namespace mold::sync::mold_sprout {

class handler {
 public:
  handler(cache& cache_, sender& sender_, sprout::handler& sprout,
          wolf::login_handler& login, configuration_handler& configurations);
  void resync();

 private:
  void connect_signals();
  void handle_with_catch(const sprout::data_types::value& value);
  void handle(const sprout::data_types::value& value);
  void send(const wolf::types::uuid_array& room_id, const sprout_value& value,
            int rowid);
  void handle_confirmed(const std::optional<std::string>& error, int rowid);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  cache& m_cache;
  sender& m_sender;
  sprout::handler& m_sprout;
  wolf::login_handler& m_login;
  configuration_handler& m_configurations;
};
}  // namespace mold::sync::mold_sprout

#endif
