#include "power_calculator.hpp"
#include <numeric>
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_metadata_creator.hpp"
#include "wolf/sensor_value_type_helper.hpp"
#include "wolf/timestamp_helper.hpp"

static constexpr auto table_name = "energy_values";

mold::power_calculator::power_calculator(
    wolf::database &database, boost::asio::io_service &io_service,
    wolf::room_handler &room_handler,
    wolf::sensor_value_handler &values_handler,
    wolf::profile_handler &profile_handler,
    const std::chrono::system_clock::duration &save_interval)
    : m_logger("power_calculator"),
      m_session(database.get_session()),
      m_room_handler(room_handler),
      m_values_handler(values_handler),
      m_profile_handler(profile_handler),
      m_room_saver(room_handler),
      m_save_timer(io_service),
      m_save_interval(save_interval) {
  m_values_handler.signal_values.connect([this](const values &sensor_values) {
    handle_new_values(sensor_values);
  });
  m_room_handler.signal_updated.connect(
      [this](const wolf::room &room) { handle_room(room); });
  m_room_handler.signal_remove_sensors.connect([this](const auto &sensor_ids) {
    remove_energy_from_database(sensor_ids);
  });
  create_tables();
  load_from_database();
}

void mold::power_calculator::handle_room_added(const wolf::room &room) {
  handle_room(room);
}

void mold::power_calculator::reset_energy(
    const mold::power_calculator::sensors &sensor_ids) {
  for (const auto &sensor : sensor_ids)
    if (wolf::sensor_value_type_helper::is_electric_energy(sensor)) {
      reset_energy(sensor);
      break;
    }
}

void mold::power_calculator::reset_energy(const wolf::sensor_id &sensor) {
  auto found = find_energy_value(sensor);
  if (found == m_energy_values.end()) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not reset energy because value not found";
    return;
  }
  const auto now = std::chrono::system_clock::now();
  found->value.value = 0.f;
  found->value.timestamp = now;
  found->start_time = now;
  save_energy_to_database();
  m_values_handler.handle_single_value(found->value);
}

std::optional<mold::energy_value::timestamp>
mold::power_calculator::get_start_time_energy(
    const wolf::sensor_id &sensor) const {
  const auto found = find_energy_value(sensor);
  if (found == m_energy_values.cend()) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not get start time for value because value not found";
    return std::optional<energy_value::timestamp>();
  }
  return found->start_time;
}

void mold::power_calculator::save_energy_to_database() {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "add or update energy values in database";
  for (const auto &energy_value : m_energy_values) {
    const std::string sensor = energy_value.value.id.to_combined_id_base64();
    const auto timestamp = wolf::timestamp_helper().cast_to_unix_seconds(
        energy_value.value.timestamp);
    const auto value_casted = static_cast<double>(energy_value.value.value);
    const auto start_time =
        wolf::timestamp_helper().cast_to_unix_seconds(energy_value.start_time);
    m_session << "insert or replace into " << table_name
              << " (sensor, value, timestamp, start_time) values(:sensor, "
                 ":value, :timestamp, :start_time)",
        soci::use(sensor), soci::use(value_casted), soci::use(timestamp),
        soci::use(start_time);
  }
}

void mold::power_calculator::handle_new_values(
    const mold::power_calculator::values &sensor_values) {
  const auto power_sum = get_power_sum(sensor_values);
  if (power_sum.has_value()) calculate_energy(power_sum.value());
  values current_values;
  std::copy_if(
      sensor_values.cbegin(), sensor_values.cend(),
      std::back_inserter(current_values), [](const auto &value) {
        return wolf::sensor_value_type_helper::is_electric_current(value.id);
      });
  if (current_values.size() == 0) return;
  handle_current_sum(current_values);
  calculate_power_values(current_values);
}

void mold::power_calculator::handle_current_sum(
    const mold::power_calculator::values &current_values) const {
  auto values_for_sum = current_values;
  remove_sum_values(values_for_sum);
  if (values_for_sum.size() == 0) return;
  calculate_sum_for_all_senders(values_for_sum);
}

void mold::power_calculator::remove_sum_values(
    mold::power_calculator::values &values_for_sum) {
  // check if one of the values already is sum
  const auto new_end = std::remove_if(
      values_for_sum.begin(), values_for_sum.end(),
      [](const wolf::sensor_value &value) { return is_current_sum(value.id); });
  values_for_sum.erase(new_end, values_for_sum.cend());
}

std::vector<wolf::types::id_esp3>
mold::power_calculator::get_different_sender_ids(
    const mold::power_calculator::values &values_for_sum) {
  std::vector<wolf::types::id_esp3> sender_ids;
  for (const auto &value : values_for_sum)
    sender_ids.push_back(wolf::sensor_id_enocean::convert_to_esp3_id(value.id));
  const auto last = std::unique(sender_ids.begin(), sender_ids.end());
  sender_ids.erase(last, sender_ids.end());
  return sender_ids;
}

void mold::power_calculator::calculate_sum_for_all_senders(
    const mold::power_calculator::values &values_for_sum) const {
  const auto sender_ids = get_different_sender_ids(values_for_sum);
  for (const auto &sender : sender_ids) {
    wolf::sensor_value to_send;
    const auto sum = std::accumulate(
        values_for_sum.begin(), values_for_sum.end(), 0.f,
        [&sender, &to_send](const float &sum_value,
                            const wolf::sensor_value &value) {
          if (wolf::sensor_id_enocean::convert_to_esp3_id(value.id) == sender) {
            to_send = value;
            return sum_value + value.value;
          }
          return sum_value;
        });
    m_values_handler.handle_single_value(
        {change_sensor_id(to_send.id, wolf::sensor_value_type::electric_current,
                          3),
         sum, to_send.timestamp});
  }
}

wolf::sensor_id mold::power_calculator::change_sensor_id(
    const wolf::sensor_id &base_id, const wolf::sensor_value_type &type,
    const std::uint8_t subtype) {
  auto new_id = base_id.get_id();
  *std::prev(new_id.end()) = subtype;
  return wolf::sensor_id(new_id, type);
}

void mold::power_calculator::handle_room(const wolf::room &room) {
  auto room_with_new_sensors = room;
  auto valid = check_if_room_has_sensor(
      room_with_new_sensors.sensors, wolf::sensor_value_type::electric_current);
  if (valid) {
    bool has_sum = check_if_room_has_current_sum(room_with_new_sensors.sensors);
    if (!has_sum) add_current_sum_sensor_to_room(room_with_new_sensors);
    bool has_energy =
        check_if_room_has_sensor(room_with_new_sensors.sensors,
                                 wolf::sensor_value_type::electric_energy);
    if (!has_energy) add_energy_sensor_to_room(room_with_new_sensors);
  }
  valid &= check_if_room_has_no_power(room_with_new_sensors.sensors);
  if (!valid) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "room has no current or already has power: "
        << room_with_new_sensors;
    return;
  }
  add_power_sensors_to_room(room_with_new_sensors);
}

bool mold::power_calculator::check_if_room_has_sensor(
    const mold::power_calculator::sensors &room_sensors,
    const wolf::sensor_value_type &to_check) {
  return std::any_of(room_sensors.cbegin(), room_sensors.cend(),
                     [&to_check](const wolf::sensor_id &sensor) {
                       return sensor.get_type() == to_check;
                     });
}

bool mold::power_calculator::check_if_room_has_no_power(
    const mold::power_calculator::sensors &room_sensors) {
  return std::none_of(
      room_sensors.cbegin(), room_sensors.cend(),
      [](const wolf::sensor_id &sensor) {
        return wolf::sensor_value_type_helper::is_electric_power(sensor);
      });
}

bool mold::power_calculator::check_if_room_has_current_sum(
    const mold::power_calculator::sensors &room_sensors) {
  return std::any_of(
      room_sensors.cbegin(), room_sensors.cend(),
      [](const wolf::sensor_id &sensor) { return is_current_sum(sensor); });
}

void mold::power_calculator::add_power_sensors_to_room(
    const wolf::room &room_to_add_to) {
  auto room = room_to_add_to;
  auto sensors = room.sensors;
  for (auto &sensor : room.sensors)
    if (wolf::sensor_value_type_helper::is_electric_current(sensor)) {
      const auto power_id = get_power_sensor_id(sensor);
      auto profile = m_profile_handler.get(sensor);
      profile.id = power_id;
      wolf::sensor_metadata_creator::current_or_power(profile.metadata,
                                                      power_id.get_id());
      m_profile_handler.learn(profile);
      sensors.push_back(power_id);
    }
  room.sensors = sensors;
  m_room_saver.save_and_update(room);
}

void mold::power_calculator::add_current_sum_sensor_to_room(
    wolf::room &room_to_add_to) const {
  auto sensors = room_to_add_to.sensors;
  for (auto &sensor : room_to_add_to.sensors)
    if (wolf::sensor_value_type_helper::is_electric_current(sensor)) {
      const auto sum_id = change_sensor_id(
          sensor, wolf::sensor_value_type::electric_current, 3);
      auto profile = m_profile_handler.get(sensor);
      profile.id = sum_id;
      wolf::sensor_metadata_creator::current_or_power(profile.metadata,
                                                      sum_id.get_id());
      m_profile_handler.learn(profile);
      sensors.push_back(sum_id);
      break;
    }
  room_to_add_to.sensors = sensors;
}

void mold::power_calculator::add_energy_sensor_to_room(
    wolf::room &room_to_add_to) const {
  auto sensors = room_to_add_to.sensors;
  for (auto &sensor : room_to_add_to.sensors)
    if (wolf::sensor_value_type_helper::is_electric_current(sensor)) {
      const auto energy_id =
          change_sensor_id(sensor, wolf::sensor_value_type::electric_energy, 8);
      sensors.push_back(energy_id);
      const auto energy_difference_id = change_sensor_id(
          sensor, wolf::sensor_value_type::electric_energy_difference, 9);
      sensors.push_back(energy_difference_id);
      break;
    }
  room_to_add_to.sensors = sensors;
}

void mold::power_calculator::calculate_power_values(
    const mold::power_calculator::values &current_values) const {
  for (const auto &value : current_values) {
    const auto new_id = get_power_sensor_id(value.id);
    // voltage of 230V, P = U * I
    const auto power_value = value.value * 230;
    m_values_handler.handle_single_value(
        {new_id, power_value, value.timestamp});
  }
}

wolf::sensor_id mold::power_calculator::get_power_sensor_id(
    const wolf::sensor_id &current_id) {
  auto power_id = current_id.get_id();
  *std::prev(power_id.end()) += 4;
  return wolf::sensor_id(power_id, wolf::sensor_value_type::electric_power);
}

bool mold::power_calculator::is_current_sum(const wolf::sensor_id &sensor) {
  return wolf::sensor_value_type_helper::is_electric_current(sensor) &&
         sensor.get_id().back() == 3;
}

std::optional<wolf::sensor_value> mold::power_calculator::get_power_sum(
    const mold::power_calculator::values &sensor_values) {
  const auto found = std::find_if(
      sensor_values.cbegin(), sensor_values.cend(),
      [](const wolf::sensor_value &value) {
        return wolf::sensor_value_type_helper::is_electric_power(value.id) &&
               value.id.get_id().back() == 7;
      });
  if (found == sensor_values.cend()) return std::optional<wolf::sensor_value>();
  return *found;
}

void mold::power_calculator::calculate_energy(
    const wolf::sensor_value &power_sum) {
  const auto energy_id = change_sensor_id(
      power_sum.id, wolf::sensor_value_type::electric_energy, 8);
  auto found = find_energy_value(energy_id);
  if (found == m_energy_values.end()) {
    add_new_energy_value(power_sum, energy_id);
    return;
  }
  change_existing_energy_value(power_sum, found);
}

void mold::power_calculator::add_new_energy_value(
    const wolf::sensor_value &power_sum, const wolf::sensor_id &id) {
  const auto now = power_sum.timestamp;
  wolf::sensor_value energy({id, 0.f, now});
  m_energy_values.push_back({energy, power_sum, now});
  m_values_handler.handle_single_value(energy);
  save_energy_to_database();
  start_timer();
}

void mold::power_calculator::change_existing_energy_value(
    const wolf::sensor_value &power_sum,
    const energy_values::iterator &energy_value) {
  const auto now = power_sum.timestamp;
  const auto last_power = energy_value->last_total_power;
  auto difference = 0.f;
  if (last_power.has_value())
    // Ws to kWh, we take 30s as a constant
    difference = 1.f / 3600000.f * (last_power->value * 30);
  const auto difference_id =
      change_sensor_id(energy_value->value.id,
                       wolf::sensor_value_type::electric_energy_difference, 9);
  const wolf::sensor_value difference_value({difference_id, difference, now});
  const auto new_value = energy_value->value.value + difference;
  const wolf::sensor_value new_energy({energy_value->value.id, new_value, now});
  energy_value->value = new_energy;
  energy_value->last_total_power = power_sum;
  m_values_handler.handle_list({new_energy, difference_value});
}

void mold::power_calculator::create_tables() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "create_tables";
  m_session << "create table if not exists " << table_name
            << " (sensor TEXT PRIMARY KEY, "
               "value REAL, "
               "timestamp INTEGER, "
               "start_time INTEGER"
               ")";
}

void mold::power_calculator::start_timer() {
  try {
    m_save_timer.expires_after(m_save_interval);
  } catch (const boost::system::system_error &error) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not set expire time to timer, message:" << error.what();
    return;
  }
  m_save_timer.async_wait(
      [this](const boost::system::error_code &error) { handle_wait(error); });
}

void mold::power_calculator::handle_wait(
    const boost::system::error_code &error) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "timer timed out with error, message:" << error.message();
    return;
  }
  save_energy_to_database();
  start_timer();
}

void mold::power_calculator::remove_energy_from_database(
    const std::vector<wolf::types::id_esp3> &sensors_) {
  energy_values to_delete;
  for (const auto &sensor_esp3 : sensors_) {
    std::copy_if(
        m_energy_values.cbegin(), m_energy_values.cend(),
        std::back_inserter(to_delete),
        [&sensor_esp3](const energy_value &energy) {
          return sensor_esp3 ==
                 wolf::sensor_id_enocean::convert_to_esp3_id(energy.value.id);
        });
  }
  if (to_delete.size() == 0) return;
  for (const auto &value_to_delete : to_delete) {
    const auto found = find_energy_value(value_to_delete.value.id);
    if (found == m_energy_values.cend()) continue;
    m_energy_values.erase(found);
    remove_from_database(value_to_delete.value.id);
  }
  if (m_energy_values.size() == 0) cancel_timer();
}

void mold::power_calculator::remove_from_database(const wolf::sensor_id &id) {
  const std::string sensor = id.to_combined_id_base64();
  m_session << "delete from " << table_name << " where sensor = :sensor",
      soci::use(sensor);
}

mold::power_calculator::energy_values::const_iterator
mold::power_calculator::find_energy_value(const wolf::sensor_id &id) const {
  return std::find_if(
      m_energy_values.cbegin(), m_energy_values.cend(),
      [&id](const energy_value &value) { return value.value.id == id; });
}

mold::power_calculator::energy_values::iterator
mold::power_calculator::find_energy_value(const wolf::sensor_id &id) {
  return std::find_if(
      m_energy_values.begin(), m_energy_values.end(),
      [&id](const energy_value &value) { return value.value.id == id; });
}

void mold::power_calculator::load_from_database() {
  soci::rowset<soci::row> rows =
      (m_session.prepare << "select sensor, value, timestamp, start_time from "
                         << table_name);
  for (const auto &row : rows) {
    energy_value entry_;
    entry_.value.id =
        wolf::sensor_id::from_combined_id_base64(row.get<std::string>(0));
    entry_.value.value = static_cast<float>(row.get<double>(1));
    entry_.value.timestamp = wolf::timestamp_helper().cast_seconds_to_chrono(
        static_cast<std::uint64_t>(row.get<int>(2)));
    entry_.start_time = wolf::timestamp_helper().cast_seconds_to_chrono(
        static_cast<std::uint64_t>(row.get<int>(3)));
    entry_.last_total_power = std::optional<wolf::sensor_value>();
    m_energy_values.push_back(entry_);
  }
  if (m_energy_values.size() > 0) start_timer();
}

void mold::power_calculator::cancel_timer() {
  boost::system::error_code error;
  m_save_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "could not cancel timer, message:" << error.message();
}
