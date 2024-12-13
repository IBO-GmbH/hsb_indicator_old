#include <wiringPi.h>

#include <QApplication>
#include <QFile>
#include <fstream>
#include <thread>

#include "log/log.hpp"
#include "log/logger.hpp"
#include "mold/beep_when_not_confirmed_handler.hpp"
#include "mold/configuration_cache.hpp"
#include "mold/controller_handler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/LanguageHandler.hpp"
#include "mold/gui/MainWindow.hpp"
#include "mold/gui/SoundOnPressHandler.hpp"
#include "mold/gui_logger.hpp"
#include "mold/heating_valve_helper.hpp"
#include "mold/heating_valve_settings_handler.hpp"
#include "mold/iccid_sender.hpp"
#include "mold/initial_room_creator.hpp"
#include "mold/median_calculator_creator.hpp"
#include "mold/mold_handler.hpp"
#include "mold/mqtt_handler.hpp"
#include "mold/nightmode_activator.hpp"
#include "mold/options_cache.hpp"
#include "mold/reset_reconnect_timer_on_signal.hpp"
#include "mold/room_updater.hpp"
#include "mold/scd30_handler.hpp"
#include "mold/sht25_handler.hpp"
#include "mold/sync/box_settings/read/handler.hpp"
#include "mold/sync/login_handler.hpp"
#include "mold/sync/resync_after_timeout.hpp"
#include "mold/sync/resync_everything.hpp"
#include "mold/sync/room/deleted/handler.hpp"
#include "mold/sync/room/settings/handler.hpp"
#include "mold/sync/room/settings/with_sensors/handler.hpp"
#include "mold/sync/sensors_values/read/handler.hpp"
#include "mold/sync/sensors_values/write/frsi.hpp"
#include "mold/sync/sensors_values/write/mold_state.hpp"
#include "mold/sync/sensors_values/write/substrate_type.hpp"
#include "version.hpp"
#include "wolf/command_handler.hpp"
#include "wolf/database.hpp"
#include "wolf/esp3_commands.hpp"
#include "wolf/hardware_address.hpp"
#include "wolf/low_pass_filter.hpp"
#include "wolf/low_pass_filter_creator.hpp"
#include "wolf/network/connector.hpp"
#include "wolf/network/login/sender.hpp"
#include "wolf/plug_n_play.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/scd30_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_rssi.hpp"
#include "wolf/sensor_type_helper.hpp"
#include "wolf/sensor_value_type_helper.hpp"
#include "wolf/serial_interface.hpp"
#include "wolf/sgpc3_handler.hpp"
#include "wolf/sht25_handler.hpp"
#include "wolf/sht85_handler.hpp"
#include "wolf/shtc1_handler.hpp"
#include "wolf/std_terminate_handler.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/tmp117_handler.hpp"
#include "wolf/tone_writer.hpp"

static wolf::types::id cast_mac_to_vector(const std::uint64_t mac_address) {
  constexpr int bits_to_output = 8 * 6;
  wolf::types::id result;
  for (int base = 0; base < bits_to_output; base += 8) {
    const wolf::types::data::value_type value =
        (mac_address >> (bits_to_output - (base + 8))) & 0xff;
    result.push_back(value);
  }
  return result;
};

static wolf::types::data get_secret(const mold::config &program_options) {
  try {
    std::ifstream file_stream(program_options.login_.secret_file,
                              std::ios::binary);
    return wolf::types::data(std::istream_iterator<std::uint8_t>(file_stream),
                             std::istream_iterator<std::uint8_t>());
  } catch (...) {
    return wolf::types::data();
  }
}

static void initialize_logging(const logging::config::all &config) {
  logging::initialize(config);
}

int main(int argc, char *argv[]) {
  using namespace wolf;
  using namespace logging;

  //  std_terminate_handler terminate_handler;

  mold::program_options_parser program_options_parser_;
  bool close{false};
  if (!program_options_parser_.parse(
          argc, argv, mold::version::get_version_and_hash(), close))
    return 1;
  if (close) return 0;

  const wolf::config &program_options = program_options_parser_.get_config();
  const mold::config &program_options_mold =
      program_options_parser_.get_config_mold();

  initialize_logging(program_options.log_);
  logging::logger main_logger{"main"};

  LOGGING_LOG(main_logger, severity::normal)
      << "parsed mac address:0x" << std::hex
      << program_options.general_.mac_address << " initial-outdoor-sensor:0x"
      << program_options_mold.initial_.outdoor_sensor << " allowed-eep:0x"
      << program_options_mold.general_.eep;

  boost::asio::io_service service;
  wolf::thread_checker::register_as_service();
  wolf::thread_checker::throw_if_not_service("mold main 1");

  std::unique_ptr<sensor_value_handler> value_handler_;
  std::unique_ptr<sensor_value_offset_handler> value_offset_handler;
  std::unique_ptr<wolf::room_cache> room_database;
  std::unique_ptr<mold::configuration_cache> configuration_cache;
  std::unique_ptr<wolf::actor::handler> actor_handler;
  std::unique_ptr<database> database_;
  std::unique_ptr<sensor_value_offset_cache> value_offset_cache;
  std::unique_ptr<wolf::profile_cache> learned_cache;
  std::unique_ptr<serial_interface> input_serial;
  std::unique_ptr<esp3_sender> esp3_writer;
  std::unique_ptr<esp3_commands> esp3_commander;
  std::unique_ptr<esp3_parser> parser_serial;
  std::unique_ptr<mold::tone_handler> tone_handler_;
  std::unique_ptr<tone_writer> tone_writer_;
  std::unique_ptr<controller::controller_cache> controller_cache;
  std::unique_ptr<controller::controller_handler> controller_handler;
  std::unique_ptr<mold::controller_handler> controller_handler_mold;
  std::unique_ptr<wolf::scd30_reader> scd30_reader;
  std::unique_ptr<wolf::scd30_handler> scd30_handler;
  std::unique_ptr<mold::scd30_handler> scd30_handler_mold;
  std::unique_ptr<wolf::sgpc3_reader> sgpc3_reader;
  std::unique_ptr<wolf::sht25_reader> sht25_reader;
  std::unique_ptr<wolf::shtc1_reader> shtc1_reader;
  std::unique_ptr<wolf::sgpc3_handler> sgpc3_handler;
  std::unique_ptr<wolf::shtc1_handler> shtc1_handler;
  std::unique_ptr<wolf::ee895_reader> ee895_reader;
  std::unique_ptr<wolf::ee895_handler> ee895_handler;
  std::unique_ptr<wolf::sht85_reader> sht85_reader;
  std::unique_ptr<wolf::sht85_handler> sht85_handler;
  std::unique_ptr<wolf::tmp117_reader> tmp117_reader;
  std::unique_ptr<wolf::tmp117_handler> tmp117_handler;
  std::unique_ptr<wolf::plug_n_play> plug_n_play;
  std::unique_ptr<mold::initial_room_creator> initial_room_creator;
  std::unique_ptr<mold::display_handler> display_handler;
  std::unique_ptr<mold::nightmode_handler> nightmode_handler;
  std::unique_ptr<mold::nightmode_activator> nightmode_activator;
  std::unique_ptr<mold::gui_logger> gui_logger_;
  std::unique_ptr<mold::DisplayPowerHandler> display_power_handler;
  std::unique_ptr<mold::SoundOnPressHandler> sound_handler;
  std::unique_ptr<QApplication> gui_application;
  std::unique_ptr<mold::MainWindow> gui_main_window;
  std::unique_ptr<mold::comfort_questionnaire_cache>
      comfort_questionnaire_cache;
  std::unique_ptr<mold::mqtt_cache> mqtt_cache;
  std::unique_ptr<mold::mqtt_handler> mqtt_handler;
  std::unique_ptr<mold::heating_valve_settings_cache>
      heating_valve_settings_cache;

  if (program_options.use_tone) {
    if (wiringPiSetup() != 0) {
      LOGGING_LOG(main_logger, severity::error) << "failed to set up wiring pi";
      return 6;
    }
  }

  database_.reset(new database());
  database_->open(program_options.database_.file);
  configuration_cache.reset(new mold::configuration_cache(*database_.get()));
  learned_cache.reset(new wolf::profile_cache(*database_.get()));
  room_database.reset(new wolf::room_cache(*database_.get()));
  value_offset_cache = std::make_unique<sensor_value_offset_cache>(*database_);

  database_transaction_helper database_transaction_helper_{*database_};
  wolf::key_value_cache key_value_cache(*database_);
  mold::options_cache options_cache(key_value_cache);
  controller_cache.reset(new controller::controller_cache(*database_));
  comfort_questionnaire_cache.reset(
      new mold::comfort_questionnaire_cache(*database_));
  mqtt_cache.reset(new mold::mqtt_cache(*database_));
  heating_valve_settings_cache.reset(
      new mold::heating_valve_settings_cache(*database_));
  std::unique_ptr<mold::beep_when_not_confirmed_handler>
      beep_when_not_confirmed;
  std::unique_ptr<mold::tone_enabled_handler> tone_enabled;

  tone_enabled.reset(new mold::tone_enabled_handler(&options_cache));

  if (program_options_mold.general_.confirm_beep_interval > 0) {
    beep_when_not_confirmed.reset(new mold::beep_when_not_confirmed_handler(
        service, std::chrono::seconds(
                     program_options_mold.general_.confirm_beep_interval)));
  }
  if (program_options.use_tone) {
    tone_writer_.reset(new tone_writer{program_options.tone_.gpio});
    if (!tone_writer_->open()) {
      LOGGING_LOG(main_logger, severity::error) << "failed to set tone writer";
      return 8;
    }
    if (program_options.tone_.duration <= 0 || program_options.tone_.pause <= 0)
      throw std::runtime_error(
          "mold main: tone duration or pause not bigger 0");
    tone_handler_.reset(new mold::tone_handler(
        service, static_cast<uint32_t>(program_options.tone_.duration),
        static_cast<uint32_t>(program_options.tone_.pause),
        *tone_writer_.get()));
    tone_handler_->set_enabled(tone_enabled->is_enabled());
    tone_enabled->signal_changed.connect([&tone_handler_](bool enabled) {
      tone_handler_->set_enabled(enabled);
    });
    if (beep_when_not_confirmed) {
      beep_when_not_confirmed->signal_beep.connect(
          [&tone_handler_] { tone_handler_->start(); });
    }
  }

  // TODO do empty implementeation of websocket_sync if false
  // if (program_options.use_network) // TODO!
  websocket_sync::websocket_values websocket_settings = {
      program_options.network_.ping_timer,
      program_options.network_.pong_timeout,
      program_options.network_.gzip_compression};
  websocket_sync network_sync{service, websocket_settings};
  wolf::network::connector::config network_connector_config{
      program_options.network_.url, program_options.general_.mac_address};
  wolf::network::connector network_connector{network_sync,
                                             network_connector_config};
  wolf::network::bond_parser bond_parser(network_sync);
  wolf::network::bond_sender bond_sender(network_sync);
  wolf::network::response::message response_message(
      bond_parser, service,
      std::chrono::seconds(program_options.network_.response_timeout));
  wolf::network::response::sender response_sender(bond_sender);
  wolf::network::response::simple response_simple(response_message);
  wolf::network::login::response login_response(response_message);
  wolf::network::login::sender login_sender(bond_sender, login_response);
  const auto mac_casted =
      cast_mac_to_vector(program_options.general_.mac_address);
  const login_handler::device_information device_information{
      program_options_mold.login_.version, mac_casted,
      get_secret(program_options_mold)};
  login_handler_implementation login(login_sender, key_value_cache,
                                     device_information);
  mold::reconnect_handler connection_handler_(service, network_sync, login,
                                              network_connector,
                                              program_options.network_);
  mold::reset_reconnect_timer_on_signal reset_reconnect_timer_on_signal(
      service, connection_handler_);
  network_sync.signal_closed.connect(
      [&response_message]() { response_message.clear(); });
  // pong time timeout? close connection!
  network_sync.signal_pong_timeout.connect(
      std::bind(&websocket_sync::close, &network_sync));
  response_message.signal_timeout.connect(
      [&network_sync] { network_sync.close(); });
  wolf::network::foreign_sensor::response foreign_sensor_response{
      response_message};
  wolf::network::foreign_sensor::requester foreign_sensor_requester{
      bond_sender, foreign_sensor_response};

  if (program_options.use_esp3) {
    input_serial.reset(new serial_interface(service));
    if (!input_serial->open(program_options.esp3_.device,
                            program_options.esp3_.baud)) {
      input_serial.reset(nullptr);
      LOGGING_LOG(main_logger, severity::error)
          << "could not initialise enocean";
    } else {
      parser_serial.reset(new esp3_parser());
      esp3_writer.reset(new esp3_sender(*input_serial.get()));
      esp3_commander.reset(new esp3_commands{*esp3_writer.get()});
      input_serial->read_async();
      esp3_commander->get_base_id();
    }
  }

  wolf::room_handler room_handler(*room_database,
                                  program_options.general_.send_sensor_rssi);
  wolf::room_name_langanguage_changer room_name_language_changer(service,
                                                                 room_handler);
  wolf::profile_handler learned{*learned_cache};
  value_offset_handler =
      std::make_unique<sensor_value_offset_handler>(*value_offset_cache);
  value_handler_ =
      std::make_unique<sensor_value_handler>(service, *value_offset_handler);
  actor_handler.reset(
      new wolf::actor::handler(esp3_writer.get(), *value_handler_));
  controller_handler.reset(new controller::controller_handler(
      *value_handler_.get(), controller_cache.get(), program_options));

  learned.signal_update.connect([&room_handler, &controller_handler](
                                    const wolf::sensor_profile &profile) {
    auto rooms = room_handler.get_all();
    for (auto &room : rooms) {
      controller_handler->reset_ventilation(room.id);
      if (profile.type == wolf::sensor_type::enocean) {
        controller_handler->reset_heating(room.id);
        controller_handler->reset_dehumidifier(room.id);
        continue;
      }
      if (sensor_value_type_helper::is_temperature(profile.id))
        controller_handler->reset_heating(room.id);
      if (sensor_value_type_helper::is_humidity(profile.id))
        controller_handler->reset_dehumidifier(room.id);
    }
  });

  mold::options_handler options_handler(
      options_cache, program_options_mold.building_level_.new_,
      program_options_mold.building_level_.old);
  mold::comfort_questionnaire_handler comfort_questionnaire_handler(
      comfort_questionnaire_cache.get(), *value_handler_, service);
  comfort_questionnaire_handler.signal_open_next_questionnaire.connect(
      [&tone_handler_]() { tone_handler_->start(true, 3); });
  mold::heating_valve_settings_handler heating_valve_settings_handler(
      *heating_valve_settings_cache);
  room_handler.signal_removed.connect(
      [&heating_valve_settings_handler](const types::uuid_array to_remove_id) {
        heating_valve_settings_handler.remove_settings(to_remove_id);
      });
  value_handler_->signal_values.connect(
      [&heating_valve_settings_handler, &main_logger, &actor_handler,
       &esp3_writer, &room_handler,
       &value_handler_](const wolf::sensor_values &values) {
        // after the heating valve sends its newest values, the settings must be
        // sent as a response immediately, as the valve is only in receive mode
        // for a short period after transmitting
        for (const auto &value : values) {
          if (!mold::heating_valve_helper::is_heating_valve(value.id)) continue;
          const auto rooms = room_handler.get_all_rooms_with_sensor(value.id);
          if (rooms.size() == 0)
            throw std::runtime_error(
                "mold: no room was found for heating valve although valve is "
                "learned!");
          if (rooms.size() > 1)
            LOGGING_LOG(main_logger, logging::severity::warning)
                << "one valve should only be connected to one room, only send "
                   "settings of first room, sensor:"
                << value.id;
          const auto room = rooms.front();
          const auto settings =
              heating_valve_settings_handler.get_options_for_room(room);
          if (!settings.has_value())
            throw std::runtime_error(
                "mold: no heating_valve_settings were found for room although "
                "valve was learned for room!");
          const auto current_temperature =
              mold::heating_valve_helper::get_current_room_temperature_value(
                  room.sensors, *value_handler_);
          const auto data_bytes = mold::heating_valve_helper::get_data_bytes(
              false, settings.value(), current_temperature);
          LOGGING_LOG(main_logger, logging::severity::verbose)
              << "heating_valve_settings to send: " << settings.value()
              << ", data_byte to send=" << std::hex << data_bytes;
          const auto base_id = actor_handler->get_base_id();
          const auto connected_valves =
              mold::heating_valve_helper::get_all_heating_valve_enocean_ids(
                  room.sensors);
          for (const auto &id : connected_valves)
            esp3_writer->send_telegram(data_bytes, RORG_4BS, id & base_id,
                                       0x00);
          return;
          // one valve sends two values, but only values from one valve are
          // handled at the same time, so it is save to return after sending the
          // settings once
        }
      });
  mold::configuration_handler configuration_handler(*configuration_cache,
                                                    key_value_cache);
  mold::configuration_values_handler configuration_values;
  const mold::configuration_state_handler::config
      configuration_state_handler_config{
          program_options_mold.mold_states_.yellow_threshold,
          program_options_mold.mold_states_.red_threshold};
  mold::configuration_state_handler configuration_states(
      key_value_cache, configuration_state_handler_config);
  mold::surface_temperature_calculator surface_temperature_calculator(
      configuration_handler, options_handler);
  mold::sprout::cache sprout_cache(key_value_cache);
  mold::sprout::handler sprout_handler(sprout_cache, configuration_values,
                                       surface_temperature_calculator);
  mold::evaluation_humidity::handler evaluation_humidity(
      surface_temperature_calculator, configuration_values,
      program_options_mold);
  mold::mold_value_handler mold_value_handler(sprout_handler,
                                              program_options_mold);
  mold::mold_handler mold_handler(configuration_handler, configuration_values,
                                  configuration_states, evaluation_humidity,
                                  sprout_handler, *value_handler_, room_handler,
                                  mold_value_handler, options_handler);

  controller_handler_mold.reset(new mold::controller_handler(
      *controller_handler, *actor_handler, room_handler, *value_handler_,
      configuration_handler, *value_offset_handler, configuration_values,
      learned));
  auto outdoor_sensor = configuration_handler.get_outdoor_sensor();
  configuration_values.set_outdoor_sensor(outdoor_sensor);
  for (const auto &config : configuration_handler.get_all())
    configuration_values.add(config);
  controller_handler->set_outdoor_sensor(outdoor_sensor);
  controller_handler->reset_all_actors();
  if (beep_when_not_confirmed) {
    configuration_states.signal_state_changed.connect(
        [&beep_when_not_confirmed](const types::uuid_array &id,
                                   const mold::calculation_state &state) {
          beep_when_not_confirmed->handle_state(id, state);
        });
    beep_when_not_confirmed->signal_confirmed.connect(
        [&configuration_states, &main_logger](const types::uuid_array &id) {
          try {
            configuration_states.handle_confirm(id);
          } catch (const std::exception &error) {
            LOGGING_LOG(main_logger, severity::error)
                << "a fatal error occured, when trying to handle the "
                   "confirm the beeping on a yellow/red mold state. what():"
                << error.what();
          }
        });
  }

  enocean_cache enocean_cache_{*database_};
  wolf::enocean_handler enocean_handler{enocean_cache_};
  enocean_handler.signal_values.connect(
      [&value_handler_, &program_options](const auto &values, const auto rssi) {
        value_handler_->handle_list(values);
        if (program_options.general_.send_sensor_rssi && rssi != 0) {
          const auto id_rssi =
              wolf::sensor_id_rssi::create_from_sensor_id(values.front().id);
          value_handler_->handle_single_value(
              {id_rssi, static_cast<float>(rssi), values.front().timestamp});
        }
      });
  enocean_handler.signal_profiles_loaded.connect(
      [&learned](const std::vector<wolf::sensor_profile> &loaded_profiles) {
        // enocean gets the profiles for all sensor types from the eep, but we
        // do not use all, so only the ones that are used should be relearned.
        // relearning is necessary for profiles which were saved without
        // metadata (old versions)
        const auto learned_profiles = learned.get_all();
        std::for_each(
            loaded_profiles.cbegin(), loaded_profiles.cend(),
            [&learned_profiles, &learned](const wolf::sensor_profile &profile) {
              for (const auto &check_profile : learned_profiles)
                if (check_profile.id == profile.id) learned.learn(profile);
            });
      });
  room_handler.signal_remove_sensors.connect(
      [&configuration_values, &enocean_handler](const auto &ids) {
        std::vector<wolf::types::id_esp3> ids_copy;
        std::copy_if(ids.begin(), ids.end(), std::back_inserter(ids_copy),
                     [&configuration_values](const auto &id) {
                       return !configuration_values.sensor_is_outdoor(id);
                     });
        enocean_handler.remove_devices(ids_copy);
      });

  scd30_reader.reset(new wolf::scd30_reader());
  sgpc3_reader.reset(new wolf::sgpc3_reader(service));
  sht25_reader.reset(new wolf::sht25_reader(service));
  shtc1_reader.reset(new wolf::shtc1_reader(service));
  ee895_reader.reset(new wolf::ee895_reader(service));
  sht85_reader.reset(new wolf::sht85_reader(service));
  tmp117_reader.reset(new wolf::tmp117_reader(service));

  mold::sht25_handler sht25_handler_(program_options, *sht25_reader);

  scd30_handler.reset(
      new wolf::scd30_handler(service, *scd30_reader, program_options));
  sgpc3_handler.reset(
      new wolf::sgpc3_handler(service, *sgpc3_reader, program_options));
  shtc1_handler.reset(new wolf::shtc1_handler(*shtc1_reader, program_options));
  ee895_handler.reset(new wolf::ee895_handler(*ee895_reader, program_options));
  sht85_handler.reset(new wolf::sht85_handler(*sht85_reader, program_options));
  tmp117_handler.reset(
      new wolf::tmp117_handler(*tmp117_reader, program_options));
  sht25_handler_.handler.signal_value.connect(
      [&value_handler_](const sensor_value &value) {
        value_handler_->handle_single_value(value);
      });
  sgpc3_handler->signal_value.connect(
      [&value_handler_](const wolf::sensor_value &value) {
        value_handler_->handle_single_value(value);
      });
  scd30_handler_mold.reset(
      new mold::scd30_handler(*scd30_handler, *value_handler_));
  shtc1_handler->signal_values.connect(
      [&value_handler_](const wolf::shtc1_handler::values_type &values) {
        value_handler_->handle_list(values);
      });
  ee895_handler->signal_values.connect(
      [&value_handler_](const wolf::ee895_handler::values_type &values) {
        value_handler_->handle_list(values);
      });
  sht85_handler->signal_values.connect(
      [&value_handler_](const wolf::sht85_handler::values_type &values) {
        value_handler_->handle_list(values);
      });
  tmp117_handler->signal_values.connect(
      [&value_handler_](const wolf::tmp117_handler::values_type &values) {
        value_handler_->handle_list(values);
      });

  mold::room_creator room_creator(room_handler, configuration_handler);
  wolf::plug_n_play::readers readers{
      *scd30_reader, *sgpc3_reader, *sht25_reader, *shtc1_reader,
      *ee895_reader, *sht85_reader, *tmp117_reader};
  plug_n_play.reset(new wolf::plug_n_play(program_options, readers));
  initial_room_creator.reset(new mold::initial_room_creator(
      program_options_mold, learned, room_creator, configuration_handler,
      enocean_handler));

  plug_n_play->signal_scd30.connect(
      [&initial_room_creator,
       &scd30_handler](const std::optional<wolf::sensor_type> &second_sensor) {
        initial_room_creator->add_scd30(second_sensor);
        scd30_handler->start_reading(second_sensor);
      });
  plug_n_play->signal_sgpc3.connect([&initial_room_creator, &sgpc3_handler]() {
    initial_room_creator->add_sgpc3();
    sgpc3_handler->start_reading();
  });
  plug_n_play->signal_sht25.connect(
      [&initial_room_creator,
       &sht25_handler_](const std::optional<wolf::sensor_type> &second_sensor) {
        if (second_sensor.has_value() &&
            *second_sensor == wolf::sensor_type::shtc1)
          return;
        initial_room_creator->add_sht25(second_sensor);
        sht25_handler_.start_reading();
      });
  plug_n_play->signal_shtc1.connect(
      [&initial_room_creator,
       &shtc1_handler](const std::optional<wolf::sensor_type> &second_sensor) {
        initial_room_creator->add_shtc1(second_sensor);
        shtc1_handler->start_reading();
      });
  plug_n_play->signal_ee895.connect(
      [&initial_room_creator,
       &ee895_handler](const std::optional<wolf::sensor_type> &second_sensor) {
        initial_room_creator->add_ee895(second_sensor);
        ee895_handler->start_reading(second_sensor);
      });
  plug_n_play->signal_sht85.connect(
      [&initial_room_creator,
       &sht85_handler](const std::optional<wolf::sensor_type> &second_sensor) {
        initial_room_creator->add_sht85(second_sensor);
        sht85_handler->start_reading();
      });
  plug_n_play->signal_tmp117.connect(
      [&initial_room_creator,
       &tmp117_handler](const std::optional<wolf::sensor_type> &second_sensor) {
        initial_room_creator->add_tmp117(second_sensor);
        tmp117_handler->start_reading();
      });
  plug_n_play->signal_none.connect(
      [&initial_room_creator]() { initial_room_creator->create_no_room(); });

  mold::network::iccid_sender iccid_sender_network(bond_sender,
                                                   response_simple);
  mold::gsm_information_reader gsm_information_reader(service,
                                                      program_options_mold);
  mold::iccid_sender iccid_sender(iccid_sender_network, login,
                                  gsm_information_reader);
  //  auto gsm_information_reader = nullptr;

  login.signal_login_error.connect([&network_sync]() { network_sync.close(); });
  wolf::network::command_parser command_parser(bond_parser, response_sender);
  command_handler command_handler_(command_parser);

  network_sync.signal_connected.connect(
      [&login] { login.handle_network_connected(); });
  network_sync.signal_closed.connect(
      [&login] { login.handle_network_closed(); });

  if (input_serial) {
    input_serial->signal_read.connect(
        std::bind(&esp3_parser::handle_data, parser_serial.get(),
                  std::placeholders::_1, std::placeholders::_2));
    parser_serial->signal_data_4bs.connect(
        [&enocean_handler, &configuration_handler](
            const wolf::types::id_esp3 &id, const types::data &data,
            const int rssi) {
          const auto outdoor_temp =
              configuration_handler.get_outdoor_sensor().temperature;
          const auto to_check = sensor_id_enocean::create(
              id, S_TEMP, std::optional<wolf::enocean_subchannel_type>(), {});

          bool is_outdoor = outdoor_temp.get_id() == to_check.get_id();
          enocean_handler.handle_value(id, data, rssi, is_outdoor);
        });
    parser_serial->signal_data_vld.connect(
        [&enocean_handler](const wolf::types::id_esp3 &id,
                           const types::data &data, const int rssi) {
          enocean_handler.handle_value(id, data, rssi, false);
        });
    parser_serial->signal_response.connect(
        [&esp3_commander](const types::data &data,
                          const types::data &optional_data) {
          esp3_commander->handle_response(data, optional_data);
        });
    esp3_commander->signal_base_id.connect(
        [&actor_handler](wolf::types::id_esp3 id) {
          actor_handler->set_base_id(id);
        });
  }

  display_handler.reset(new mold::display_handler(program_options_mold, service,
                                                  configuration_states));
  nightmode_handler.reset(new mold::nightmode_handler(
      program_options_mold, &options_cache, display_handler.get(), service));
  nightmode_activator.reset(new mold::nightmode_activator(
      program_options_mold, *nightmode_handler.get(), service));
  nightmode_handler->signal_enabled.connect(
      [&nightmode_activator](const bool enabled) {
        if (!enabled) {
          nightmode_activator->deactivate();
          return;
        }
        nightmode_activator->activate(std::chrono::system_clock::now());
      });
  if (program_options.use_tone) {
    tone_handler_->set_nightmode_active(nightmode_handler->is_active());
    nightmode_handler->signal_active.connect(
        [&tone_handler_](const bool active) {
          tone_handler_->set_nightmode_active(active);
        });
  }

  mold::room_saver room_saver_{room_handler};
  mold::configuration_saver configuration_saver_{configuration_handler};
  mold::room_and_configuration_saver room_configuration_saver{
      room_saver_, configuration_saver_};

  mold::sync::room::cache room_sync_cache(*database_);
  mold::sync::room::settings::with_sensors::sender
      room_settings_with_sensors_sender(bond_sender, response_simple);
  mold::sync::room::settings::with_sensors::handler
      room_settings_with_sensors_handler(
          room_sync_cache, room_settings_with_sensors_sender, room_handler,
          configuration_handler, login, learned);

  mold::sync::room::deleted::sender room_deleted_sender(bond_sender,
                                                        response_simple);
  mold::sync::room::deleted::handler room_deleted_handler(
      room_sync_cache, room_deleted_sender, room_handler, login);

  mold::sync::room::settings::parser room_settings_parser(bond_parser,
                                                          response_sender);
  mold::sync::room::settings::handler room_settings_handler(
      room_sync_cache, room_settings_parser, room_settings_with_sensors_handler,
      room_handler, configuration_handler);

  mold::sync::box_settings::cache box_settings_cache(*database_);
  mold::sync::box_settings::write::sender box_settings_write_sender(
      bond_sender, response_simple, learned,
      program_options.general_.send_sensor_rssi);
  mold::sync::box_settings::write::handler box_settings_write_handler(
      box_settings_cache, box_settings_write_sender, login,
      configuration_handler, options_handler);
  mold::sync::box_settings::read::parser box_settings_read_parser(
      bond_parser, response_sender);
  mold::outdoor_sensor_change_helper outdoor_sensor_setter(
      configuration_handler, room_handler, room_configuration_saver);
  mold::sync::box_settings::read::handler box_settings_read_handler(
      box_settings_read_parser, box_settings_cache, box_settings_write_handler,
      options_handler, learned, outdoor_sensor_setter);

  mold::sync::sensors_values::write::cache sensors_values_cache(*database_);
  mold::sync::sensors_values::write::sender sensors_values_sender(
      bond_sender, response_simple);
  mold::sync::sensors_values::write::handler sync_sensors_values_handler(
      sensors_values_cache, sensors_values_sender, *value_handler_, login);
  room_handler.signal_remove_values_for_sensors.connect(
      [&sync_sensors_values_handler, &room_handler,
       &configuration_handler](const auto &sensors) {
        const auto all_rooms = room_handler.get_all();
        const auto outdoor = configuration_handler.get_outdoor_sensor();
        sync_sensors_values_handler.handle_sensors_to_remove(sensors, all_rooms,
                                                             outdoor);
      });
  const mold::sync::sensors_values::write::mold_state sensors_values_mold_state(
      sync_sensors_values_handler, configuration_states, outdoor_sensor_setter,
      room_settings_with_sensors_handler);
  const mold::sync::sensors_values::write::frsi sensors_values_frsi(
      sync_sensors_values_handler, options_handler, configuration_handler,
      room_settings_with_sensors_handler);
  const mold::sync::sensors_values::write::substrate_type
      sensors_values_substrate_type(sync_sensors_values_handler,
                                    options_handler, configuration_handler,
                                    room_settings_with_sensors_handler);

  mold::sync::mold_sprout::cache sync_sprout_cache(*database_);
  mold::sync::mold_sprout::sender_implementation sync_sprout_sender(
      bond_sender, response_simple);
  mold::sync::mold_sprout::handler sync_sprout_handler(
      sync_sprout_cache, sync_sprout_sender, sprout_handler, login,
      configuration_handler);

  mold::sync::sensors_values::read::parser sensors_values_read_parser(
      bond_parser, response_sender, learned);
  mold::sync::sensors_values::read::handler sensors_values_read_handler(
      sensors_values_read_parser, *value_handler_);

  const mold::sync::resync_everything resync_everything(
      login, box_settings_write_handler, room_settings_with_sensors_handler);
  const mold::sync::resync_after_timeout resync_after_timeout(
      service, login, box_settings_write_handler, room_deleted_handler,
      room_settings_with_sensors_handler, sync_sensors_values_handler,
      sync_sprout_handler,
      std::chrono::seconds(program_options_mold.general_.resync_timeout));
  const mold::sync::login_handler resync_after_login(
      login, box_settings_write_handler, room_deleted_handler,
      room_settings_with_sensors_handler, sync_sensors_values_handler,
      sync_sprout_handler);

  mold::initial_room_creator::handlers handlers{
      *scd30_handler, *sgpc3_handler, sht25_handler_, *shtc1_handler,
      *ee895_handler, *sht85_handler, *tmp117_handler};
  mold::room_updater room_updater(room_saver_, configuration_handler,
                                  configuration_saver_);

  mold::reset_handler reset_handler;
  reset_handler.signal_reset_values.connect([&sync_sensors_values_handler] {
    sync_sensors_values_handler.reset_values_to_send();
  });

  mold::power_calculator power_calculator(
      *database_, service, room_handler, *value_handler_, learned,
      std::chrono::minutes(program_options_mold.general_.energy_save_interval));

  comfort_questionnaire_handler.signal_change_questionnaire_sensors.connect(
      [&program_options_mold, &room_handler, &learned, &room_updater,
       &main_logger](const bool add,
                     const std::vector<sensor_id> &questionnaire_sensors) {
        // store questionnaire sensors in intial room or in the first room (if
        // there is no initial room)
        wolf::uuid_helper helper;
        const auto initial_id(
            helper.string_to_array(program_options_mold.initial_.room_id));
        wolf::room room;
        try {
          room = room_handler.get(initial_id);
        } catch (std::runtime_error &) {
          LOGGING_LOG(main_logger, severity::verbose)
              << "no initial room, use first room";
          auto all_rooms = room_handler.get_all();
          if (all_rooms.empty()) {
            LOGGING_LOG(main_logger, severity::warning)
                << "there are no rooms!";
            return;
          }
          room = all_rooms.front();
        }
        auto sensors = room.sensors;
        if (add) {
          wolf::sensor_metadata_creator::questionnaire_metadata metadata;
          const bool already_learned =
              learned.is_learned(questionnaire_sensors[0]);
          if (!already_learned) {
            metadata = wolf::sensor_metadata_creator::questionnaire();
            if (metadata.size() != questionnaire_sensors.size())
              throw std::runtime_error(
                  "mold::change_questionnaire_sensors: sensor size unequal "
                  "metadata size");
          }
          const bool sensors_already_in_room = std::any_of(
              sensors.begin(), sensors.end(), [](wolf::sensor_id &sensor) {
                return wolf::sensor_type_helper::is_comfort_questionnaire(
                    sensor);
              });
          if (sensors_already_in_room) return;
          for (unsigned int index = 0; index < questionnaire_sensors.size();
               ++index) {
            if (!already_learned)
              learned.learn({questionnaire_sensors[index],
                             wolf::sensor_type::comfort_questionnaire,
                             metadata[index]});
            sensors.push_back(questionnaire_sensors[index]);
          }
        } else {
          sensors.erase(
              std::remove_if(
                  sensors.begin(), sensors.end(),
                  [](wolf::sensor_id &sensor) {
                    return wolf::sensor_type_helper::is_comfort_questionnaire(
                        sensor);
                  }),
              sensors.end());
        }
        room.sensors = sensors;
        room_updater.update_room_and_config(room);
      });

  {
    plug_n_play->check_sensors();
    auto cached_rooms = room_database->get_all();
    if (cached_rooms.empty() || cached_rooms.front().sensors.empty()) {
      auto initial_room = initial_room_creator->create_initial_room(
          configuration_handler.get_outdoor_sensor().is_set());
      if (initial_room.has_value())
        initial_room_creator->resend_all_signals(initial_room.get(), handlers);
    } else {
      auto changed_room = initial_room_creator->change_initial_room(
          cached_rooms, configuration_handler.get_outdoor_sensor().is_set());
      if (changed_room.has_value()) {
        if (changed_room.get().sensors.empty()) {
          room_handler.remove(changed_room->id);
          const auto config_removed =
              configuration_handler.remove_by_room_id(changed_room->id);
          if (!config_removed)
            LOGGING_LOG(main_logger, severity::warning)
                << "config could not be deleted for room_id: "
                << changed_room->id;
        } else if (room_updater.update_room_and_config(changed_room.get()))
          initial_room_creator->resend_all_signals(changed_room.get(),
                                                   handlers);
      }
    }
    comfort_questionnaire_handler.add_sensors_if_necessary();
  }
  network_connector.connect();
  mqtt_handler.reset(
      new mold::mqtt_handler({program_options.general_.mac_address,
                              program_options.mqtt_.broker_ip_address,
                              program_options.mqtt_.broker_port},
                             *mqtt_cache, room_creator, room_updater, service,
                             *value_handler_, room_handler));
  room_handler.signal_removed.connect(
      [&mqtt_handler](const types::uuid_array to_remove_id) {
        mqtt_handler->remove_client(to_remove_id);
      });

  auto close_services =
      [&main_logger, &close, &input_serial, &scd30_reader, &scd30_handler,
       &sgpc3_handler, &sgpc3_reader, &sht25_handler_, &shtc1_reader,
       &ee895_reader, &connection_handler_, &network_sync, &database_,
       &gui_application, &response_message, &beep_when_not_confirmed,
       &nightmode_handler, &nightmode_activator, &power_calculator,
       &sht25_reader, &comfort_questionnaire_handler]() {
        LOGGING_LOG(main_logger, severity::normal) << "closing all services";
        try {
          close = true;
          if (gui_application) {
            QMetaObject::invokeMethod(gui_application.get(), "quit",
                                      Qt::QueuedConnection);
          }
          power_calculator.save_energy_to_database();
          response_message.clear();
          if (input_serial) {
            input_serial->close();
          }
          scd30_reader->close();
          scd30_handler->close();
          sgpc3_reader->close();
          sgpc3_handler->close();
          sht25_handler_.close();
          sht25_reader->close();
          shtc1_reader->close();
          ee895_reader->close();
          if (database_) {
            database_->close();
          }
          connection_handler_.close();
          network_sync.close();
          if (beep_when_not_confirmed) {
            beep_when_not_confirmed->cancel();
          }
          nightmode_handler->close();
          nightmode_activator->deactivate();
          power_calculator.cancel_timer();
          comfort_questionnaire_handler.close();
        } catch (std::runtime_error &error) {
          std::cerr << error.what() << std::endl;
          return;
        }
      };

  mold::WolfServices services = {&service,
                                 &login,
                                 &configuration_handler,
                                 &configuration_values,
                                 &configuration_states,
                                 &mold_value_handler,
                                 actor_handler.get(),
                                 value_handler_.get(),
                                 &room_handler,
                                 &learned,
                                 &options_handler,
                                 parser_serial.get(),
                                 esp3_writer.get(),
                                 &enocean_handler,
                                 &network_sync,
                                 program_options.general_.mac_address,
                                 tone_handler_.get(),
                                 tone_enabled.get(),
                                 &program_options,
                                 &program_options_mold,
                                 controller_handler.get(),
                                 display_handler.get(),
                                 nightmode_handler.get(),
                                 &room_configuration_saver,
                                 value_offset_handler.get(),
                                 &foreign_sensor_requester,
                                 &connection_handler_,
                                 &gsm_information_reader,
                                 &room_name_language_changer,
                                 &reset_handler,
                                 &sync_sensors_values_handler,
                                 &outdoor_sensor_setter,
                                 &power_calculator,
                                 &comfort_questionnaire_handler,
                                 &heating_valve_settings_handler};

  mold::DataExchange data_exchange{services};
  data_exchange.initialise();

  auto initialise_and_run_ui = [&]() {
    wolf::thread_checker::register_as_ui();
    wolf::thread_checker::throw_if_not_ui("mold main");
    LOGGING_LOG(main_logger, severity::normal) << "running ui";
    gui_logger_.reset(new mold::gui_logger());
    QLocale::setDefault(
        QLocale(QLocale::Language::German, QLocale::Country::Austria));
    gui_application.reset(new QApplication(argc, argv));
    if (!program_options_mold.gui_.style.empty()) {
      QStyle *style = gui_application->setStyle(
          QString::fromStdString(program_options_mold.gui_.style));
      if (style == nullptr) {
        LOGGING_LOG(main_logger, severity::error)
            << "failed to set gui style:" << program_options_mold.gui_.style;
        return;
      }
    }
    QFile stylesheetFile(
        QString::fromStdString(program_options_mold.general_.stylesheet));
    if (!stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      throw std::invalid_argument(
          "pathToStyleSheet:" + program_options_mold.general_.stylesheet +
          " could not get opened");
    }
    const QByteArray content = stylesheetFile.readAll();
    gui_application->setStyleSheet(QString::fromLatin1(content));

    // TODO instance inside MainWindow!
    mold::LanguageHandler languages(services, data_exchange, key_value_cache,
                                    program_options_mold.general_.language,
                                    program_options_mold.general_.language_de);
    mold::RoomNameHandler room_name_handler(data_exchange);
    mold::Co2StateHandler co2_state_handler(services);
    mold::RoomRotationHandler room_rotation_handler(services,
                                                    co2_state_handler);
    mold::SensorActivityHandler sensor_activity(
        program_options, configuration_handler.get_outdoor_sensor(),
        data_exchange);
    gui_main_window.reset(
        new mold::MainWindow(room_name_handler, room_rotation_handler,
                             co2_state_handler, sensor_activity));
    gui_main_window->show();
    display_power_handler.reset(new mold::DisplayPowerHandler(
        &data_exchange, services, co2_state_handler));
    if (beep_when_not_confirmed) {
      display_power_handler->signal_there_was_input.connect(
          [&beep_when_not_confirmed, &service] {
            service.post([&beep_when_not_confirmed] {
              beep_when_not_confirmed->confirm_all();
            });
          });
    }
    gui_application->installEventFilter(display_power_handler.get());
    sound_handler.reset(new mold::SoundOnPressHandler(services));
    gui_application->installEventFilter(sound_handler.get());
    if (!close) {
      // TODO remove initialise?
      gui_main_window->initialise(services, data_exchange,
                                  display_power_handler.get(), &languages);
      LOGGING_LOG(main_logger, severity::normal) << "running ui exec";
      int result = gui_application->exec();
      LOGGING_LOG(main_logger, severity::normal)
          << "ui exec returned, with result:" << result;
    }
    gui_main_window.reset();
    gui_application.reset();
  };

  auto signal_handler = [&main_logger, &close_services](
                            const boost::system::error_code &error,
                            int signal_number) {
    if (error) {
      LOGGING_LOG(main_logger, severity::warning)
          << "signal_handler called with error:" << error.message();
      return;
    }
    LOGGING_LOG(main_logger, severity::normal)
        << "signal_handler called with signal:" << std::dec << signal_number;
    close_services();
  };
  boost::asio::signal_set signals_to_listen_to(service, SIGINT, SIGTERM);
  signals_to_listen_to.async_wait(signal_handler);

  if (program_options_mold.use_gui) {
    std::thread background_thread([&]() {
      wolf::thread_checker::register_as_service();
      wolf::thread_checker::throw_if_not_service("mold main 2");
      service.run();
      LOGGING_LOG(main_logger, severity::verbose) << "end of background thread";
    });
    initialise_and_run_ui();
    LOGGING_LOG(main_logger, severity::verbose) << "joining background thread";
    background_thread.join();
    LOGGING_LOG(main_logger, severity::verbose)
        << "done joining background thread";
  } else {
    service.run();
  }

  return 0;
}
