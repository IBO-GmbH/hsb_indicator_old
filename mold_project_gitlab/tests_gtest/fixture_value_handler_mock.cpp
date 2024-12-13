#include "fixture_value_handler_mock.hpp"

#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;

static wolf::sensor_value_offset_handler &make_value_offset_handler() {
  // TODO refactor. this is soooo dirty. use boost::di instead!
  static std::shared_ptr<database_in_memory> database;
  static std::shared_ptr<wolf::sensor_value_offset_cache>
      sensor_value_offset_cache;
  static std::shared_ptr<wolf::sensor_value_offset_handler>
      value_offset_handler;
  database = std::make_shared<database_in_memory>();
  sensor_value_offset_cache =
      std::make_shared<wolf::sensor_value_offset_cache>(*database);
  value_offset_handler = std::make_shared<wolf::sensor_value_offset_handler>(
      *sensor_value_offset_cache);
  return *value_offset_handler.get();
}

fixture_value_handler_mock::fixture_value_handler_mock(
    boost::asio::io_service &io_service)
    : wolf::sensor_value_handler(io_service, make_value_offset_handler()) {
  check_humidtiy = 30.f;
  check_temperature = 20.7f;
  check_co2 = 10.f;
  check_sprout_value = 10.f;
  check_voc = 47.f;
}

void fixture_value_handler_mock::save_value(const wolf::sensor_value &value) {
  wolf::sensor_value_handler::save_value(value);
  EXPECT_EQ(value.value, check_value);
}

void fixture_value_handler_mock::set_check_value(const float value) {
  check_value = value;
}

boost::optional<float> fixture_value_handler_mock::get_value_float(
    const wolf::sensor_id &id) const {
  if (wolf::sensor_value_type_helper::is_humidity(id)) return check_humidtiy;
  if (wolf::sensor_value_type_helper::is_temperature(id))
    return check_temperature;
  if (wolf::sensor_value_type_helper::is_co2(id)) return check_co2;
  if (wolf::sensor_value_type_helper::is_sprout(id)) return check_sprout_value;
  return check_voc;
}

void fixture_value_handler_mock::set_humidity(const float humidity) {
  check_humidtiy = humidity;
}

void fixture_value_handler_mock::set_temperature(const float temperature) {
  check_temperature = temperature;
}

void fixture_value_handler_mock::set_co2(const float co2) { check_co2 = co2; }

void fixture_value_handler_mock::set_sprout_value(const float sprout_value) {
  check_sprout_value = sprout_value;
}

void fixture_value_handler_mock::set_voc(const float voc) { check_voc = voc; }
