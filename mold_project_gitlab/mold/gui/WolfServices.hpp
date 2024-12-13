#ifndef MOLD_GUI_WOLFSERVICES
#define MOLD_GUI_WOLFSERVICES

#include "mold/comfort_questionnaire_handler.hpp"
#include "mold/display_handler.hpp"
#include "mold/gsm_information_reader.hpp"
#include "mold/heating_valve_settings_handler.hpp"
#include "mold/mold_value_handler.hpp"
#include "mold/nightmode_handler.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "mold/power_calculator.hpp"
#include "mold/reconnect_handler.hpp"
#include "mold/reset_handler.hpp"
#include "mold/room_and_configuration_saver.hpp"
#include "mold/sync/sensors_values/write/handler.hpp"
#include "mold/tone_handler.hpp"
#include "wolf/actor/handler.hpp"
#include "wolf/controller/controller_handler.hpp"
#include "wolf/enocean_handler.hpp"
#include "wolf/esp3_parser.hpp"
#include "wolf/network/foreign_sensor/requester.hpp"
#include "wolf/room_name_langanguage_changer.hpp"

namespace mold {

struct WolfServices {
  boost::asio::io_service* service;
  wolf::login_handler* login_handler;
  mold::configuration_handler* configurations;
  mold::configuration_values_handler* configuration_values;
  mold::configuration_state_handler* configuration_states;
  mold::mold_value_handler* mold_value_handler_;
  wolf::actor::handler* actor_handler;
  wolf::sensor_value_handler* value_handler;
  wolf::room_handler* rooms;
  wolf::profile_handler* learn_handler;
  mold::options_handler* options_handler;
  wolf::esp3_parser* esp3_parser;
  wolf::esp3_sender* esp3_writer;
  wolf::enocean_handler* enocean_handler;
  wolf::websocket_sync* websocket;
  std::uint64_t mac_address;
  mold::tone_handler* tone_handler;
  mold::tone_enabled_handler* tone_enabled_handler;
  const wolf::config* config_wolf;
  const mold::config* config;
  wolf::controller::controller_handler* controller_handler;
  mold::display_handler* display_handler;
  mold::nightmode_handler* nightmode_handler;
  mold::room_and_configuration_saver* room_and_configuration_saver;
  wolf::sensor_value_offset_handler* sensor_value_offset_handler;
  wolf::network::foreign_sensor::requester* foreign_sensor_requester;
  mold::reconnect_handler* reconnect_handler;
  mold::gsm_information_reader* gsm_information_reader;
  wolf::room_name_langanguage_changer* room_name_language_changer;
  mold::reset_handler* reset_handler;
  mold::sync::sensors_values::write::handler* sensor_values_handler_sync;
  mold::outdoor_sensor_change_helper* outdoor_change_helper;
  mold::power_calculator* power_calculator_;
  mold::comfort_questionnaire_handler* comfort_questionnaire_handler;
  mold::heating_valve_settings_handler* heating_valve_settings_handler;
};
}  // namespace mold

#endif  // MOLD_GUI_WOLFSERVICES
