#ifndef MOLD_SYNC_RESYNC_EVERYTHING_HPP
#define MOLD_SYNC_RESYNC_EVERYTHING_HPP

#include "mold/sync/box_settings/write/handler.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "wolf/login_handler.hpp"

namespace mold::sync {

class resync_everything {
 public:
  resync_everything(wolf::login_handler& login,
                    box_settings::write::handler& box_settings_,
                    room::settings::with_sensors::handler& room_);

 private:
  void connect_signals();
  void handle_with_catch();
  void handle();

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_resync;
  wolf::login_handler& m_login;
  box_settings::write::handler& m_box_settings;
  room::settings::with_sensors::handler& m_room;
};

}  // namespace mold::sync

#endif
