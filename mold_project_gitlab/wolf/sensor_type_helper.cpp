#include "sensor_type_helper.hpp"

wolf::sensor_type wolf::sensor_type_helper::get_type(
    const wolf::sensor_id &id) {
  if (id.get_type() == sensor_value_type::frsi) return sensor_type::frsi;
  if (id.get_type() == sensor_value_type::sprout)
    return sensor_type::sprout_value;
  if (id.get_type() == sensor_value_type::substrate)
    return sensor_type::substrate;
  if (id.get_type() == sensor_value_type::mold_state)
    return sensor_type::mold_state;
  if (id.get_type() == sensor_value_type::evaluation_humidity)
    return sensor_type::evaluation_humidity;
  // this method can only be used for the not hashed ids
  auto type_as_uint = id.get_id().front();
  return static_cast<sensor_type>(type_as_uint);
}

bool wolf::sensor_type_helper::is_none(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::none;
}

bool wolf::sensor_type_helper::is_enocean(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::enocean;
}

bool wolf::sensor_type_helper::is_sht25(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::sht25;
}

bool wolf::sensor_type_helper::is_shtc1(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::shtc1;
}

bool wolf::sensor_type_helper::is_sprout_value(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::sprout_value;
}

bool wolf::sensor_type_helper::is_sgpc3(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::sgpc3;
}

bool wolf::sensor_type_helper::is_scd30(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::scd30;
}

bool wolf::sensor_type_helper::is_ee895(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::ee895;
}

bool wolf::sensor_type_helper::is_open_weather(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::open_weather;
}

bool wolf::sensor_type_helper::is_evaluation_humidity(
    const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::evaluation_humidity;
}

bool wolf::sensor_type_helper::is_foreign(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::foreign;
}

bool wolf::sensor_type_helper::is_mold_state(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::mold_state;
}

bool wolf::sensor_type_helper::is_frsi(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::frsi;
}

bool wolf::sensor_type_helper::is_substrate(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::substrate;
}

bool wolf::sensor_type_helper::is_sensor_rssi(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::sensor_rssi;
}

bool wolf::sensor_type_helper::is_comfort_questionnaire(
    const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::comfort_questionnaire;
}

bool wolf::sensor_type_helper::is_sht85(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::sht85;
}

bool wolf::sensor_type_helper::is_tmp117(const wolf::sensor_id &id) {
  return get_type(id) == sensor_type::tmp117;
}
