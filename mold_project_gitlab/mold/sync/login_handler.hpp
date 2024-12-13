#ifndef MOLD_SYNC_LOGIN_HANDLER_HPP
#define MOLD_SYNC_LOGIN_HANDLER_HPP

#include "mold/sync/box_settings/write/handler.hpp"
#include "mold/sync/mold_sprout/handler.hpp"
#include "mold/sync/room/deleted/handler.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "mold/sync/sensors_values/write/handler.hpp"
#include "wolf/login_handler.hpp"

namespace mold::sync {

class login_handler {
 public:
  login_handler(wolf::login_handler& login,
                box_settings::write::handler& box_settings_,
                room::deleted::handler& deleted,
                room::settings::with_sensors::handler& room_,
                sensors_values::write::handler& values,
                mold_sprout::handler& mold_sprout_);

 private:
  void connect_signals();
  void handle_logged_in_with_catch(const bool logged_in);
  void handle_logged_in(const bool logged_in_);

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_login;
  wolf::login_handler& m_login;

  box_settings::write::handler& m_box_settings;
  room::deleted::handler& m_deleted;
  room::settings::with_sensors::handler& m_room;
  sensors_values::write::handler& m_values;
  mold_sprout::handler& m_mold_sprout;
};

}  // namespace mold::sync

#endif
