#include <boost/exception/diagnostic_information.hpp>
#include <fstream>
#include "log/log.hpp"
#include "mold/mold_handler.hpp"
#include "mold/mold_value_handler.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/reset_reconnect_timer_on_signal.hpp"
#include "mold/room_and_configuration_saver.hpp"
#include "mold/sensor_to_room_and_configuration_helper.hpp"
#include "mold/sync/box_settings/read/handler.hpp"
#include "mold/sync/login_handler.hpp"
#include "mold/sync/resync_after_timeout.hpp"
#include "mold/sync/resync_everything.hpp"
#include "mold/sync/room/settings/handler.hpp"
#include "mold/sync/sensors_values/read/handler.hpp"
#include "mold/sync/sensors_values/write/frsi.hpp"
#include "mold/sync/sensors_values/write/mold_state.hpp"
#include "mold/version.hpp"
#include "wolf/command_handler.hpp"
#include "wolf/login_handler.hpp"
#include "wolf/network/command_parser.hpp"
#include "wolf/network/connector.hpp"
#include "wolf/network/response/sender.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_shtc1.hpp"
#include "wolf/sensor_value_handler.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/websocket_sync.hpp"

using timestamp = std::chrono::system_clock::time_point;
using hours = std::chrono::hours;

static constexpr float temperature_indoors = 25;
/*array with humidities, indexes:
0: border for 1 day
1: border for 2 days
2: border for 4 days
3: border for 8 days
4: border for 16 days
humidities were calculated in Excel sheet to match borders*/
static constexpr std::array<float, 5> humidity_indoors = {{90, 85, 80, 78, 75}};
static constexpr float humidity_indoors_no_danger = 10;
static constexpr float temperature_outdoors = 20;
static constexpr float humidity_outdoors = 70;
static constexpr float frsi = 0.71f;

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

static void initialize_logging() {
  logging::config::all config;
  config.console_.enabled = true;
  logging::initialize(config);
}

static timestamp increase_time_per_hours(const timestamp &to_increase,
                                         const int hours_to_add) {
  return to_increase + hours(hours_to_add);
}

static timestamp increase_time_per_days(const timestamp &to_increase,
                                        const int days) {
  return increase_time_per_hours(to_increase, 24 * days);
}

// every curve rises to 100% and then falls to 0%, one after another
static timestamp test_scenario_1(
    wolf::sensor_value_handler &sensor_value_handler,
    const wolf::sensor_id &indoor_hum, const timestamp &start) {
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[0], start});

  auto new_start = increase_time_per_days(start, 1);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  new_start = increase_time_per_days(new_start, 1);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[1], new_start});

  new_start = increase_time_per_days(new_start, 2);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  new_start = increase_time_per_days(new_start, 2);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[2], new_start});

  new_start = increase_time_per_days(new_start, 4);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  new_start = increase_time_per_days(new_start, 4);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[3], new_start});

  new_start = increase_time_per_days(new_start, 8);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  new_start = increase_time_per_days(new_start, 8);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[4], new_start});

  new_start = increase_time_per_days(new_start, 16);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  new_start = increase_time_per_days(new_start, 16);
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors_no_danger, new_start});

  return new_start;
}

static timestamp send_value_every_hour(
    wolf::sensor_value_handler &sensor_value_handler,
    const wolf::sensor_id &indoor_hum, const float value,
    const timestamp &start, const int hours_to_run) {
  auto new_start = increase_time_per_hours(start, 1);
  for (int count = 1; count < hours_to_run; ++count) {
    sensor_value_handler.handle_single_value({indoor_hum, value, new_start});
    new_start = increase_time_per_hours(new_start, 1);
  }
  return new_start;
}

static int days_to_hours(const float days) {
  return static_cast<int>(24 * days);
}

// curve 4 rises -> surpassed by curve 2 -> surpassed by curve 1 -> all curves
// fall -> curve 1 falls into curve 2 -> curve 2 falls into curve 4 -> curve 4
// falls to 0%
static timestamp test_scenario_2(
    wolf::sensor_value_handler &sensor_value_handler,
    const wolf::sensor_id &indoor_hum, const timestamp &start) {
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[2], start});

  auto new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[2], start, days_to_hours(0.5f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors[1], new_start,
                                    days_to_hours(0.5f));
  new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[0], new_start, days_to_hours(1.f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors_no_danger, new_start,
                                    days_to_hours(2.f));

  return new_start;
}

// curve 4 rises -> surpassed by curve 2 -> surpassed by curve 1 -> all curves
// fall -> curve 1 starts rising again
static timestamp test_scenario_3(
    wolf::sensor_value_handler &sensor_value_handler,
    const wolf::sensor_id &indoor_hum, const timestamp &start) {
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[2], start});

  auto new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[2], start, days_to_hours(0.5f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors[1], new_start,
                                    days_to_hours(0.5f));
  new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[0], new_start, days_to_hours(1.f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors_no_danger, new_start,
                                    days_to_hours(0.5f));
  new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[0], new_start, days_to_hours(1.f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors_no_danger, new_start,
                                    days_to_hours(2.5f));

  return new_start;
}

// curve 1 rises to 100% -> falls to rising curve 2 -> curve 2 rises to 100% ->
// falls to rising curve 4 -> rises to 100% -> falls to 0%
static timestamp test_scenario_4(
    wolf::sensor_value_handler &sensor_value_handler,
    const wolf::sensor_id &indoor_hum, const timestamp &start) {
  sensor_value_handler.handle_single_value(
      {indoor_hum, humidity_indoors[0], start});

  auto new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[0], start, days_to_hours(1.f));
  new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[1], new_start, days_to_hours(1.f));
  new_start =
      send_value_every_hour(sensor_value_handler, indoor_hum,
                            humidity_indoors[2], new_start, days_to_hours(2.f));
  new_start = send_value_every_hour(sensor_value_handler, indoor_hum,
                                    humidity_indoors_no_danger, new_start,
                                    days_to_hours(4.f));

  return new_start;
}

int main(int argc, char *argv[]) {
  try {
    mold::program_options_parser program_options_parser_;
    bool close{false};
    if (!program_options_parser_.parse(
            argc, argv, mold::version::get_version_and_hash(), close))
      return 1;
    if (close) return 0;

    const wolf::config &program_options = program_options_parser_.get_config();
    const mold::config &program_options_mold =
        program_options_parser_.get_config_mold();

    initialize_logging();
    logging::logger main_logger{"mold_values_test"};

    boost::asio::io_service service;
    wolf::thread_checker::register_as_service();
    wolf::thread_checker::throw_if_not_service("mold_values_test");

    wolf::database database("database.db");
    wolf::sensor_value_offset_cache offset_cache(database);
    wolf::sensor_value_offset_handler offset_handler(offset_cache);
    wolf::sensor_value_handler sensor_value_handler(service, offset_handler);
    wolf::key_value_cache key_value_cache(database);
    mold::sprout::cache sprout_cache(key_value_cache);
    mold::configuration_values_handler config_values_handler;
    mold::configuration_cache config_cache(database);
    mold::configuration_handler config_handler(config_cache, key_value_cache);
    mold::options_cache options_cache(key_value_cache);
    mold::options_handler options_handler(
        options_cache, program_options_mold.building_level_.new_,
        program_options_mold.building_level_.old);
    mold::surface_temperature_calculator surface_temp(config_handler,
                                                      options_handler);
    mold::sprout::handler sprout_handler(sprout_cache, config_values_handler,
                                         surface_temp);
    mold::mold_value_handler mold_value_handler(sprout_handler,
                                                program_options_mold);
    const mold::configuration_state_handler::config
        configuration_state_handler_config{
            program_options_mold.mold_states_.yellow_threshold,
            program_options_mold.mold_states_.red_threshold};
    mold::configuration_state_handler config_states_handler(
        key_value_cache, configuration_state_handler_config);
    mold::evaluation_humidity::handler eval_hum_handler(
        surface_temp, config_values_handler, program_options_mold);
    wolf::room_cache room_cache(database);
    wolf::room_handler room_handler(room_cache, false);
    mold::mold_handler mold_handler(
        config_handler, config_values_handler, config_states_handler,
        eval_hum_handler, sprout_handler, sensor_value_handler, room_handler,
        mold_value_handler, options_handler);
    wolf::profile_cache profile_cache(database);
    wolf::profile_handler profile_handler(profile_cache);

    wolf::websocket_sync::websocket_values websocket_settings = {
        program_options.network_.ping_timer,
        program_options.network_.pong_timeout,
        program_options.network_.gzip_compression};
    wolf::websocket_sync network_sync{service, websocket_settings};
    wolf::network::connector::config network_connector_config{
        "wss://testcloud.umidus.com:4714/?device_id=<device_id>",
        program_options.general_.mac_address};
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
    const wolf::login_handler::device_information device_information{
        program_options_mold.login_.version, mac_casted,
        get_secret(program_options_mold)};
    wolf::login_handler_implementation login(login_sender, key_value_cache,
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
        std::bind(&wolf::websocket_sync::close, &network_sync));
    response_message.signal_timeout.connect(
        [&network_sync] { network_sync.close(); });

    mold::room_saver room_saver_{room_handler};
    mold::configuration_saver configuration_saver_{config_handler};
    mold::room_and_configuration_saver room_configuration_saver{
        room_saver_, configuration_saver_};

    mold::sync::room::cache room_sync_cache(database);
    mold::sync::room::settings::with_sensors::sender
        room_settings_with_sensors_sender(bond_sender, response_simple);
    mold::sync::room::settings::with_sensors::handler
        room_settings_with_sensors_handler(
            room_sync_cache, room_settings_with_sensors_sender, room_handler,
            config_handler, login, profile_handler);

    mold::sync::room::deleted::sender room_deleted_sender(bond_sender,
                                                          response_simple);
    mold::sync::room::deleted::handler room_deleted_handler(
        room_sync_cache, room_deleted_sender, room_handler, login);

    mold::sync::room::settings::parser room_settings_parser(bond_parser,
                                                            response_sender);
    mold::sync::room::settings::handler room_settings_handler(
        room_sync_cache, room_settings_parser,
        room_settings_with_sensors_handler, room_handler, config_handler);

    mold::sync::box_settings::cache box_settings_cache(database);
    mold::sync::box_settings::write::sender box_settings_write_sender(
        bond_sender, response_simple, profile_handler, false);
    mold::sync::box_settings::write::handler box_settings_write_handler(
        box_settings_cache, box_settings_write_sender, login, config_handler,
        options_handler);
    mold::sync::box_settings::read::parser box_settings_read_parser(
        bond_parser, response_sender);
    mold::outdoor_sensor_change_helper outdoor_sensor_setter(
        config_handler, room_handler, room_configuration_saver);
    mold::sync::box_settings::read::handler box_settings_read_handler(
        box_settings_read_parser, box_settings_cache,
        box_settings_write_handler, options_handler, profile_handler,
        outdoor_sensor_setter);

    mold::sync::sensors_values::write::cache sensors_values_cache(database);
    mold::sync::sensors_values::write::sender sensors_values_sender(
        bond_sender, response_simple);
    mold::sync::sensors_values::write::handler sync_sensors_values_handler(
        sensors_values_cache, sensors_values_sender, sensor_value_handler,
        login);
    const mold::sync::sensors_values::write::mold_state
        sensors_values_sensors_values(
            sync_sensors_values_handler, config_states_handler,
            outdoor_sensor_setter, room_settings_with_sensors_handler);
    const mold::sync::sensors_values::write::frsi sensors_values_frsi(
        sync_sensors_values_handler, options_handler, config_handler,
        room_settings_with_sensors_handler);

    mold::sync::mold_sprout::cache sync_sprout_cache(database);
    mold::sync::mold_sprout::sender_implementation sync_sprout_sender(
        bond_sender, response_simple);
    mold::sync::mold_sprout::handler sync_sprout_handler(
        sync_sprout_cache, sync_sprout_sender, sprout_handler, login,
        config_handler);

    mold::sync::sensors_values::read::parser sensors_values_read_parser(
        bond_parser, response_sender, profile_handler);
    mold::sync::sensors_values::read::handler sensors_values_read_handler(
        sensors_values_read_parser, sensor_value_handler);

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

    login.signal_login_error.connect(
        [&network_sync]() { network_sync.close(); });
    wolf::network::command_parser command_parser(bond_parser, response_sender);
    wolf::command_handler command_handler_(command_parser);

    network_sync.signal_connected.connect(
        [&login] { login.handle_network_connected(); });
    network_sync.signal_closed.connect(
        [&login] { login.handle_network_closed(); });

    const auto indoor_temp =
        wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
    const auto indoor_hum =
        wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
    const std::vector<wolf::sensor_id> room_sensors{indoor_temp, indoor_hum};
    mold::sensor_to_room_and_configuration_helper helper;
    auto room_and_config =
        room_configuration_saver.save_and_update({{}, "Test Room 1", {}}, {});
    helper.handle(room_sensors, room_and_config.first, room_and_config.second,
                  true);
    room_configuration_saver.save_and_update(room_and_config.first,
                                             room_and_config.second);

    wolf::outdoor_sensor outdoor;
    outdoor.temperature = wolf::sensor_id_enocean::create(
        1, S_TEMP, std::optional<wolf::enocean_subchannel_type>(), {});
    outdoor.humidity = wolf::sensor_id_enocean::create(
        1, S_RELHUM, std::optional<wolf::enocean_subchannel_type>(), {});
    config_handler.set_outdoor_sensor(outdoor);
    config_values_handler.set_outdoor_sensor(outdoor);

    const auto start = std::chrono::system_clock::now();
    sensor_value_handler.handle_single_value(
        {outdoor.temperature, temperature_outdoors, start});
    sensor_value_handler.handle_single_value(
        {outdoor.humidity, humidity_outdoors, start});
    options_handler.set_building_type_and_frsi(mold::building_type::new_, frsi);
    options_handler.set_substrate_type(mold::substrate_type::type_two);
    sensor_value_handler.handle_single_value(
        {indoor_temp, temperature_indoors, start});

    auto start_for_next_scenario =
        test_scenario_1(sensor_value_handler, indoor_hum, start);

    start_for_next_scenario = test_scenario_2(sensor_value_handler, indoor_hum,
                                              start_for_next_scenario);

    start_for_next_scenario = test_scenario_3(sensor_value_handler, indoor_hum,
                                              start_for_next_scenario);

    start_for_next_scenario = test_scenario_4(sensor_value_handler, indoor_hum,
                                              start_for_next_scenario);

    network_connector.connect();

    service.run();

    database.close();
    return 0;
  } catch (const std::runtime_error &error) {
    std::cout << boost::diagnostic_information(error) << std::endl;
    return 1;
  }
}
