#include "wolf/controller/sensor_value_getter.hpp"
#include "fixture_value_handler_mock.hpp"
#include "gtest/gtest.h"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_sgpc3.hpp"
#include "wolf/sensor_value_type_helper.hpp"

namespace sensor_value_getter_test {

static logging::logger test_logger("sensor_value_getter_test");

static wolf::sensor_id create_enocean_sensor_id(const std::uint8_t &id_digit,
                                                const CHANNEL_TYPE &channel) {
  return wolf::sensor_id_enocean::create(
      id_digit, channel, std::optional<wolf::enocean_subchannel_type>(), {});
}

TEST(TestSensorValueGetter, GetHumidityNoError) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  const auto sensor = create_enocean_sensor_id(1, S_RELHUM);
  const auto value = getter.get_humidity(sensor);
  EXPECT_TRUE(value);
  EXPECT_EQ(value, handler.check_humidtiy);
}

TEST(TestSensorValueGetter, GetHumidityWrongType) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, S_TEMP);
  EXPECT_FALSE(getter.get_humidity(sensor).is_initialized());
}

TEST(TestSensorValueGetter, GetTemperatureNoError) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, S_TEMP);
  const auto value = getter.get_temperature(sensor);
  EXPECT_TRUE(value);
  EXPECT_EQ(value, handler.check_temperature);
}

TEST(TestSensorValueGetter, GetTemperatureWrongType) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, S_CONC);
  EXPECT_FALSE(getter.get_temperature(sensor).is_initialized());
}

TEST(TestSensorValueGetter, GetCO2NoError) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, S_CONC);
  const auto value = getter.get_co2(sensor);
  EXPECT_TRUE(value);
  EXPECT_EQ(value, handler.check_co2);
}

TEST(TestSensorValueGetter, GetCO2WrongType) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, E_VOC);
  EXPECT_FALSE(getter.get_co2(sensor).is_initialized());
}

TEST(TestSensorValueGetter, GetVOCNoError) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  const auto sensor =
      wolf::sensor_id_sgpc3::create(wolf::sensor_sgpc3_channel::tvoc);
  const auto value = getter.get_voc(sensor);
  EXPECT_TRUE(value);
  EXPECT_EQ(value, handler.check_voc);
}

TEST(TestSensorValueGetter, GetVOCWrongType) {
  boost::asio::io_service service;
  fixture_value_handler_mock handler(service);
  wolf::controller::sensor_value_getter getter(handler);
  auto sensor = create_enocean_sensor_id(1, S_RELHUM);
  EXPECT_FALSE(getter.get_voc(sensor).is_initialized());
}
}  // namespace sensor_value_getter_test
