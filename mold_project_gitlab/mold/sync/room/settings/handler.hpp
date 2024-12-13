#ifndef MOLD_SYNC_ROOM_SETTINGS_HANDLER_HPP
#define MOLD_SYNC_ROOM_SETTINGS_HANDLER_HPP

#include "mold/configuration_handler.hpp"
#include "mold/sync/room/settings/parser.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "wolf/room_handler.hpp"

namespace mold::sync::room::settings {

class handler {
 public:
  handler(cache& cache_, parser& parser_, with_sensors::handler& sync_sender,
          wolf::room_handler& rooms, configuration_handler& configurations);

 private:
  void connect_signals();
  void handle_message_with_catch(wolf::network::response::send_helper& response,
                                 const parser::information& information);
  void handle_message(wolf::network::response::send_helper& response,
                      const parser::information& information);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  cache& m_cache;
  parser& m_parser;
  with_sensors::handler& m_sync_sender;
  wolf::room_handler& m_rooms;
  configuration_handler& m_configurations;
};

}  // namespace mold::sync::room::settings

#endif
