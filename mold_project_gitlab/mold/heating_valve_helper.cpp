#include "heating_valve_helper.hpp"
#include <algorithm>
#include "wolf/sensor_id_enocean.hpp"

#include <bitset>
#include <cmath>

bool mold::heating_valve_helper::is_heating_valve(const wolf::sensor_id &id) {
  const auto type = id.get_type();
  return type == wolf::sensor_value_type::valve_temperature ||
         type == wolf::sensor_value_type::valve_position;
}

uint8_t mold::heating_valve_helper::calculate_temperature_byte(
    const float temperature) {
  // temperature range from 0...40°C, value range from 0...255
  return static_cast<std::uint8_t>(std::round(temperature * 255.f / 40.f));
}

uint8_t mold::heating_valve_helper::get_byte_zero() {
  // data telegram has bit 4 set to one, all others are set to zero
  return 0x08;
}

uint8_t mold::heating_valve_helper::get_byte_one(const bool init,
                                                 const bool summer_mode,
                                                 const bool manual) {
  std::uint8_t init_byte = static_cast<std::uint8_t>(init);
  std::uint8_t summer_byte = static_cast<std::uint8_t>(summer_mode);
  std::uint8_t manual_byte = static_cast<std::uint8_t>(!manual);
  std::uint8_t byte_one = static_cast<std::uint8_t>(
      manual_byte << 2 | summer_byte << 3 | init_byte << 7);
  return byte_one;
}

uint8_t mold::heating_valve_helper::get_byte_two(
    const std::optional<float> &current_temperature) {
  // temperature range from 0...40°C, value range from 1...255, 0x00 tells the
  // valve to use the internal temperature sensor
  if (!current_temperature.has_value()) return 0x00;
  return static_cast<std::uint8_t>(
      std::round(current_temperature.value() * (-254.f) / 40.f + 255.f));
}

uint8_t mold::heating_valve_helper::get_byte_three(
    const float temperature, const uint8_t &valve_position, const bool manual) {
  if (manual) return valve_position;
  return calculate_temperature_byte(temperature);
}

mold::heating_valve_helper::enocean_ids
mold::heating_valve_helper::get_all_heating_valve_enocean_ids(
    const mold::heating_valve_helper::sensor_ids &sensors) {
  enocean_ids valve_ids;
  for (const auto &id : sensors) {
    if (!is_heating_valve(id)) continue;
    const auto enocean_id = wolf::sensor_id_enocean::convert_to_esp3_id(id);
    if (!is_enocean_id_in_list(enocean_id, valve_ids))
      valve_ids.push_back(enocean_id);
  }
  return valve_ids;
}

bool mold::heating_valve_helper::is_enocean_id_in_list(
    const wolf::types::id_esp3 &id,
    const mold::heating_valve_helper::enocean_ids &list) {
  return std::find(list.begin(), list.end(), id) != list.end();
}

mold::heating_valve_helper::sensor_ids
mold::heating_valve_helper::get_all_heating_valve_sensor_ids(
    const mold::heating_valve_helper::sensor_ids &sensors) {
  const auto valve_enocean_ids = get_all_heating_valve_enocean_ids(sensors);
  sensor_ids valve_sensor_ids;
  std::copy_if(
      sensors.cbegin(), sensors.cend(), std::back_inserter(valve_sensor_ids),
      [&valve_enocean_ids](const wolf::sensor_id &id) {
        const auto enocean_id = wolf::sensor_id_enocean::convert_to_esp3_id(id);
        return is_enocean_id_in_list(enocean_id, valve_enocean_ids);
      });
  return valve_sensor_ids;
}

wolf::sensor_values mold::heating_valve_helper::get_heating_valve_values(
    const wolf::sensor_values &all_values, const wolf::types::id_esp3 &id) {
  wolf::sensor_values values;
  std::copy_if(all_values.cbegin(), all_values.cend(),
               std::back_inserter(values),
               [&id](const wolf::sensor_value &value) {
                 const auto value_id =
                     wolf::sensor_id_enocean::convert_to_esp3_id(value.id);
                 return value_id == id;
               });
  return values;
}

wolf::types::data mold::heating_valve_helper::get_data_bytes(
    const bool init, const mold::heating_valve_settings &settings,
    const std::optional<float> &current_temperature) {
  const auto byte_zero = get_byte_zero();
  const auto byte_one =
      get_byte_one(init, settings.summer_mode, settings.manual_mode);
  const auto byte_two = get_byte_two(current_temperature);
  const auto byte_three =
      get_byte_three(settings.desired_temperature,
                     static_cast<std::uint8_t>(settings.set_valve_position),
                     settings.manual_mode);
  wolf::types::data data_byte;
  data_byte.push_back(byte_three);
  data_byte.push_back(byte_two);
  data_byte.push_back(byte_one);
  data_byte.push_back(byte_zero);
  return data_byte;
}

wolf::types::data mold::heating_valve_helper::get_learn_response_bytes() {
  // func of heating valve, 6 bit
  std::bitset<24> func(0x20);
  // type of heating valve, 7 bit
  std::bitset<24> type(0x01);
  // manufacturer id used for hsb box, same as heating valve used here,
  // necessary for teach in response, 11 bit
  std::bitset<24> manufacturer_id(0x062);
  // byte to send for successfull teach in response
  std::uint8_t successfull_teach_byte = 0xF0;
  std::bitset<24> byte_three_to_one =
      (func << (24 - 6)) | (type << (24 - 6 - 7)) | manufacturer_id;
  std::bitset<8> byte_three;
  std::bitset<8> byte_two;
  std::bitset<8> byte_one;
  for (unsigned int index = 0; index < byte_three.size(); ++index) {
    byte_one[index] = byte_three_to_one[index];
    byte_two[index] = byte_three_to_one[index + 8];
    byte_three[index] = byte_three_to_one[index + 16];
  }
  wolf::types::data data_byte;
  data_byte.push_back(static_cast<std::uint8_t>(byte_three.to_ulong()));
  data_byte.push_back(static_cast<std::uint8_t>(byte_two.to_ulong()));
  data_byte.push_back(static_cast<std::uint8_t>(byte_one.to_ulong()));
  data_byte.push_back(successfull_teach_byte);
  return data_byte;
}

std::optional<float>
mold::heating_valve_helper::get_current_room_temperature_value(
    const mold::heating_valve_helper::sensor_ids &sensors,
    wolf::sensor_value_handler &value_handler) {
  const auto sensor_id = get_current_room_temperature_sensor(sensors);
  if (!sensor_id.has_value()) return std::optional<float>();
  const auto value = value_handler.get_value_float(sensor_id.value());
  if (!value.has_value())
    throw std::runtime_error(
        "heating_valve_helper::get_current_room_temperature_value: no value "
        "was found although sensor is in room!");
  return value.value();
}

std::optional<wolf::sensor_id>
mold::heating_valve_helper::get_current_room_temperature_sensor(
    const mold::heating_valve_helper::sensor_ids &sensors) {
  const auto found = std::find_if(
      sensors.cbegin(), sensors.cend(), [](const wolf::sensor_id &id) {
        return id.get_type() == wolf::sensor_value_type::temperature;
      });
  if (found == sensors.cend()) return std::optional<wolf::sensor_id>();
  return *found;
}
