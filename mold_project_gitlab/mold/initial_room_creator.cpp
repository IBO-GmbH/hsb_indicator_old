#include "initial_room_creator.hpp"

#include "mold/heating_valve_helper.hpp"
#include "mold/program_options_parser.hpp"
#include "mold/room_creator.hpp"
#include "wolf/onboard_sensor_checker.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_id_ee895.hpp"
#include "wolf/sensor_id_scd30.hpp"
#include "wolf/sensor_id_sgpc3.hpp"
#include "wolf/sensor_id_sht25.hpp"
#include "wolf/sensor_id_sht85.hpp"
#include "wolf/sensor_id_shtc1.hpp"
#include "wolf/sensor_id_tmp117.hpp"
#include "wolf/sensor_type_helper.hpp"

using namespace logging;

mold::initial_room_creator::initial_room_creator(
    const mold::config &program_options_,
    wolf::profile_handler &profile_handler_, room_creator &room_creator_,
    mold::configuration_handler &configuration_handler_,
    wolf::enocean_handler &enocean_handler)
    : m_logger("initial_room_creator"),
      m_creator(room_creator_),
      m_program_options(program_options_),
      m_profile_handler(profile_handler_),
      m_configuration_handler(configuration_handler_),
      m_enocean_handler(enocean_handler),
      m_initial_id(
          m_helper.string_to_array(m_program_options.initial_.room_id)) {}

mold::initial_room_creator::optional_room
mold::initial_room_creator::create_initial_room(const bool outdoor_is_set) {
  LOGGING_LOG(m_logger, logging::severity::normal) << "create_initial_room";
  if (m_room_sensors.empty()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "no onboard sensors, no room created";
    return optional_room();
  }
  if (!outdoor_is_set) add_initial_outdoor_sensor();
  const auto result = m_creator.create(wolf::types::uuid_array{},
                                       m_program_options.initial_.room_name,
                                       m_room_sensors, m_initial_id, true);
  return result.room;
}

mold::initial_room_creator::optional_room
mold::initial_room_creator::change_initial_room(const rooms &rooms_,
                                                const bool outdoor_is_set) {
  auto room = get_initial_room(rooms_);
  if (!room.has_value()) return create_initial_room(outdoor_is_set);
  if (room->sensors == m_room_sensors) return optional_room();
  const auto valve_sensors =
      heating_valve_helper::get_all_heating_valve_sensor_ids(room->sensors);
  room->sensors.clear();
  m_room_sensors.insert(m_room_sensors.cend(), valve_sensors.cbegin(),
                        valve_sensors.cend());
  room->sensors = m_room_sensors;
  return optional_room(room);
}

void mold::initial_room_creator::add_scd30(
    const std::optional<wolf::sensor_type> &second_sensor) {
  m_indoor_humidity = get_humidity_sensor(second_sensor);
  auto co2 = get_co2_sensor(second_sensor);
  if (!co2.has_value())
    co2 = wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::co2);
  const auto metadata = wolf::sensor_metadata_creator::scd30(second_sensor);
  if (!second_sensor.has_value() ||
      (second_sensor.has_value() &&
       *second_sensor != wolf::sensor_type::tmp117))
    add_scd30_and_ee895_temperature_sensor(
        second_sensor,
        wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::temperature),
        metadata);
  m_profile_handler.learn(
      {m_indoor_humidity, wolf::sensor_type_helper::get_type(m_indoor_humidity),
       metadata.humidity_or_air_pressure});
  m_profile_handler.learn(
      {*co2, wolf::sensor_type_helper::get_type(*co2), metadata.co2});
  add_sensor_to_room_if_not_already_added(m_indoor_humidity);
  add_sensor_to_room_if_not_already_added(*co2);
}

void mold::initial_room_creator::add_sgpc3() {
  const wolf::sensor_id voc =
      wolf::sensor_id_sgpc3::create(wolf::sensor_sgpc3_channel::tvoc);
  m_profile_handler.learn(
      {voc, wolf::sensor_type::sgpc3, wolf::sensor_metadata_creator::spgc3()});
  add_sensor_to_room_if_not_already_added(voc);
}

void mold::initial_room_creator::add_ee895(
    const std::optional<wolf::sensor_type> &second_sensor) {
  auto co2 = wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::co2);
  const auto air_pressure =
      wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::air_pressure);
  const auto metadata = wolf::sensor_metadata_creator::ee895(second_sensor);
  m_profile_handler.learn({co2, wolf::sensor_type::ee895, metadata.co2});
  add_sensor_to_room_if_not_already_added(co2);
  if (!second_sensor.has_value() ||
      (second_sensor.has_value() &&
       *second_sensor != wolf::sensor_type::tmp117))
    add_scd30_and_ee895_temperature_sensor(
        second_sensor,
        wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::temperature),
        metadata);
  m_profile_handler.learn({air_pressure, wolf::sensor_type::ee895,
                           metadata.humidity_or_air_pressure});
  add_sensor_to_room_if_not_already_added(air_pressure);
}

void mold::initial_room_creator::add_sht85(
    const std::optional<wolf::sensor_type> &second_sensor) {
  // will be refactored in issue 715
  if (!second_sensor.has_value()) {
    m_indoor_temperature =
        wolf::sensor_id_sht85::create(wolf::sensor_sht85_channel::temperature);
    m_indoor_humidity =
        wolf::sensor_id_sht85::create(wolf::sensor_sht85_channel::humidity);
    handle_sht25_and_shtc1_sensors(wolf::sensor_type::sht85, second_sensor);
    return;
  }
  if (*second_sensor == wolf::sensor_type::ee895 ||
      *second_sensor == wolf::sensor_type::tmp117) {
    m_indoor_humidity =
        wolf::sensor_id_sht85::create(wolf::sensor_sht85_channel::humidity);
    const auto metadata = wolf::sensor_metadata_creator::sht85();
    m_profile_handler.learn({m_indoor_humidity, wolf::sensor_type::sht85,
                             metadata.humidity_or_air_pressure});
    add_sensor_to_room_if_not_already_added(m_indoor_humidity);
  }
}

void mold::initial_room_creator::add_tmp117(
    const std::optional<wolf::sensor_type> & /*second_sensor*/) {
  // will be refactored in issue 715
  m_indoor_temperature =
      wolf::sensor_id_tmp117::create(wolf::sensor_tmp117_channel::temperature);
  m_profile_handler.learn({m_indoor_temperature, wolf::sensor_type::tmp117,
                           wolf::sensor_metadata_creator::tmp117()});
  add_sensor_to_room_if_not_already_added(m_indoor_temperature);
}

void mold::initial_room_creator::add_sht25(
    const std::optional<wolf::sensor_type> &second_sensor) {
  if (second_sensor.has_value() && *second_sensor == wolf::sensor_type::shtc1) {
    add_shtc1({});
    return;
  }
  // will be refactored in issue 715
  if (!second_sensor.has_value() ||
      (second_sensor.has_value() &&
       *second_sensor != wolf::sensor_type::tmp117))
    m_indoor_temperature =
        wolf::sensor_id_sht25::create(wolf::sensor_sht25_channel::temperature);
  m_indoor_humidity =
      wolf::sensor_id_sht25::create(wolf::sensor_sht25_channel::humidity);
  handle_sht25_and_shtc1_sensors(wolf::sensor_type::sht25, second_sensor);
}

void mold::initial_room_creator::add_shtc1(
    const std::optional<wolf::sensor_type> &second_sensor) {
  // will be refactored in issue 715
  if (!second_sensor.has_value() ||
      (second_sensor.has_value() &&
       *second_sensor != wolf::sensor_type::tmp117))
    m_indoor_temperature =
        wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
  m_indoor_humidity =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
  handle_sht25_and_shtc1_sensors(wolf::sensor_type::shtc1, second_sensor);
}

void mold::initial_room_creator::create_no_room() {
  if (!m_room_sensors.empty()) m_room_sensors.clear();
}

void mold::initial_room_creator::resend_all_signals(
    const wolf::room &room, mold::initial_room_creator::handlers &handlers_) {
  if (wolf::onboard_sensor_checker::has_sht25(room))
    handlers_.sht25_handler_.resend_signals();
  if (wolf::onboard_sensor_checker::has_shtc1(room))
    handlers_.shtc1_handler_.resend_signals();
  if (wolf::onboard_sensor_checker::has_scd30(room))
    handlers_.scd30_handler_.resend_signals();
  if (wolf::onboard_sensor_checker::has_sgpc3(room))
    handlers_.sgpc3_handler_.resend_signal();
  if (wolf::onboard_sensor_checker::has_ee895(room))
    handlers_.ee895_handler_.resend_signals();
  if (wolf::onboard_sensor_checker::has_sht85(room))
    handlers_.sht85_handler_.resend_signals();
  if (wolf::onboard_sensor_checker::has_tmp117(room))
    handlers_.tmp117_handler_.resend_signals();
}

void mold::initial_room_creator::add_initial_outdoor_sensor() {
  if (m_program_options.initial_.outdoor_sensor == 0x00) return;
  const wolf::types::id_esp3 id = m_program_options.initial_.outdoor_sensor;
  const wolf::types::eep eep = m_program_options.general_.eep;
  add_outdoor_sensor(id, eep);
}

void mold::initial_room_creator::add_outdoor_sensor(
    const wolf::types::id_esp3 sensor, const wolf::types::eep &eep) {
  const auto profiles = m_enocean_handler.register_(sensor, eep, true);
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "add_outdoor_sensor, sensor:" << sensor << ", eep:" << eep
      << ", profiles:" << profiles;
  wolf::outdoor_sensor outdoor;
  for (const auto &profile : profiles) {
    m_profile_handler.learn(profile);
    if (profile.id.get_type() == wolf::sensor_value_type::temperature)
      outdoor.temperature = profile.id;
    if (profile.id.get_type() == wolf::sensor_value_type::humidity)
      outdoor.humidity = profile.id;
  }
  m_configuration_handler.set_outdoor_sensor(outdoor);
}

mold::initial_room_creator::optional_room
mold::initial_room_creator::get_initial_room(const rooms &rooms_) const {
  for (const auto &room : rooms_)
    if (m_initial_id == room.id) return room;
  return optional_room();
}

std::optional<wolf::sensor_id>
mold::initial_room_creator::get_temperature_sensor(
    const std::optional<wolf::sensor_type> &second_sensor) const {
  if (second_sensor.has_value()) {
    if (*second_sensor == wolf::sensor_type::sht25)
      return wolf::sensor_id_sht25::create(
          wolf::sensor_sht25_channel::temperature);
    if (*second_sensor == wolf::sensor_type::shtc1)
      return wolf::sensor_id_shtc1::create(
          wolf::sensor_shtc1_channel::temperature);
    if (*second_sensor == wolf::sensor_type::scd30)
      return wolf::sensor_id_scd30::create(
          wolf::sensor_scd30_channel::temperature);
  }
  return {};
}

wolf::sensor_id mold::initial_room_creator::get_humidity_sensor(
    const std::optional<wolf::sensor_type> &second_sensor) const {
  if (second_sensor.has_value()) {
    if (*second_sensor == wolf::sensor_type::sht25)
      return wolf::sensor_id_sht25::create(
          wolf::sensor_sht25_channel::humidity);
    if (*second_sensor == wolf::sensor_type::shtc1)
      return wolf::sensor_id_shtc1::create(
          wolf::sensor_shtc1_channel::humidity);
  }
  return wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::humidity);
}

std::optional<wolf::sensor_id> mold::initial_room_creator::get_co2_sensor(
    const std::optional<wolf::sensor_type> &second_sensor) const {
  if (second_sensor.has_value()) {
    if (*second_sensor == wolf::sensor_type::scd30)
      return wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::co2);
    if (*second_sensor == wolf::sensor_type::ee895)
      return wolf::sensor_id_ee895::create(wolf::sensor_ee895_channel::co2);
  }
  return {};
}

void mold::initial_room_creator::add_scd30_and_ee895_temperature_sensor(
    const std::optional<wolf::sensor_type> &second_sensor,
    const wolf::sensor_id &standard_id_temperature,
    const wolf::onboard_metadata &metadata) {
  auto indoor_temperature = get_temperature_sensor(second_sensor);
  if (!indoor_temperature.has_value())
    indoor_temperature = standard_id_temperature;
  m_indoor_temperature = *indoor_temperature;
  m_profile_handler.learn(
      {m_indoor_temperature,
       wolf::sensor_type_helper::get_type(m_indoor_temperature),
       metadata.temperature});
  add_sensor_to_room_if_not_already_added(m_indoor_temperature);
}

void mold::initial_room_creator::add_sht25_and_shtc1_co2_sensor(
    const std::optional<wolf::sensor_type> &second_sensor,
    const wolf::onboard_metadata &metadata) {
  auto co2 = get_co2_sensor(second_sensor);
  if (!co2.has_value()) return;
  m_profile_handler.learn(
      {*co2, wolf::sensor_type_helper::get_type(*co2), metadata.co2});
  add_sensor_to_room_if_not_already_added(*co2);
}

void mold::initial_room_creator::add_sensor_to_room_if_not_already_added(
    const wolf::sensor_id &to_add) {
  if (std::find_if(m_room_sensors.cbegin(), m_room_sensors.cend(),
                   [to_add](const wolf::sensor_id &id) {
                     return to_add == id;
                   }) != m_room_sensors.cend())
    return;
  m_room_sensors.push_back(to_add);
}

void mold::initial_room_creator::handle_sht25_and_shtc1_sensors(
    const wolf::sensor_type &sensor,
    const std::optional<wolf::sensor_type> &second_sensor) {
  wolf::onboard_metadata metadata;
  if (sensor == wolf::sensor_type::sht25)
    metadata = wolf::sensor_metadata_creator::sht25(second_sensor);
  else if (sensor == wolf::sensor_type::shtc1)
    metadata = wolf::sensor_metadata_creator::shtc1(second_sensor);
  // will be refactored in issue 715
  else if (sensor == wolf::sensor_type::sht85)
    metadata = wolf::sensor_metadata_creator::sht85();
  m_profile_handler.learn(
      {m_indoor_humidity, sensor, metadata.humidity_or_air_pressure});
  add_sensor_to_room_if_not_already_added(m_indoor_humidity);
  if (second_sensor.has_value() && *second_sensor == wolf::sensor_type::tmp117)
    return;
  m_profile_handler.learn({m_indoor_temperature, sensor, metadata.temperature});
  add_sensor_to_room_if_not_already_added(m_indoor_temperature);
  add_sht25_and_shtc1_co2_sensor(second_sensor, metadata);
}
