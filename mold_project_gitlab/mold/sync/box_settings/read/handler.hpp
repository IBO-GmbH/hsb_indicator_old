#ifndef MOLD_SYNC_BOX_SETTINGS_READ_HANDLER_HPP
#define MOLD_SYNC_BOX_SETTINGS_READ_HANDLER_HPP

#include "mold/options_handler.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "mold/sync/box_settings/cache.hpp"
#include "mold/sync/box_settings/read/parser.hpp"
#include "mold/sync/box_settings/write/handler.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "wolf/profile_handler.hpp"

namespace mold::sync::box_settings::read {

class handler {
 public:
  handler(parser& parser_, cache& cache_, write::handler& writer,
          options_handler& options, wolf::profile_handler& profiles,
          outdoor_sensor_change_helper& outdoor_sensor_setter);

 private:
  void connect_signals();
  void handle_message_with_catch(wolf::network::response::send_helper& response,
                                 const entity& information);
  void handle_message(wolf::network::response::send_helper& response,
                      const entity& information);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  parser& m_parser;
  cache& m_cache;
  write::handler& m_writer;
  options_handler& m_options;
  wolf::profile_handler& m_profiles;
  outdoor_sensor_change_helper& m_outdoor_sensor_setter;
};

}  // namespace mold::sync::box_settings::read
#endif
