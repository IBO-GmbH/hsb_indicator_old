#include "wolf/sensor_value_handler.hpp"
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "wolf/enocean_link.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_sht25.hpp"

namespace sensor_value_handler_test {

using time_point = std::chrono::system_clock::time_point;

static logging::logger test_logger("sensor_value_handler_test");

wolf::sensor_id create_sensor_id_sht25(
    const wolf::sensor_sht25_channel &channel) {
  wolf::sensor_id sensor = wolf::sensor_id_sht25::create(channel);
  return sensor;
}

wolf::sensor_id create_sensor_id_enocean(const wolf::types::id_esp3 &id,
                                         const CHANNEL_TYPE &channel) {
  wolf::sensor_id sensor = wolf::sensor_id_enocean::create(
      id, channel, std::optional<wolf::enocean_subchannel_type>(), {});
  return sensor;
}

// humidity==true --> humidity
// humidity==false --> temperature
wolf::sensor_id test_handle(wolf::sensor_value_handler &handler,
                            const wolf::sensor_type &sensor_type,
                            const wolf::types::id_esp3 &id, const float value,
                            const bool humidity) {
  wolf::sensor_id sensor;
  if (sensor_type == wolf::sensor_type::sht25) {
    if (humidity)
      sensor = create_sensor_id_sht25(wolf::sensor_sht25_channel::humidity);
    else
      sensor = create_sensor_id_sht25(wolf::sensor_sht25_channel::temperature);
  }
  if (sensor_type == wolf::sensor_type::enocean) {
    if (humidity)
      sensor = create_sensor_id_enocean(id, S_RELHUM);
    else
      sensor = create_sensor_id_enocean(id, S_TEMP);
  }
  const auto now = std::chrono::system_clock::now();
  EXPECT_NO_THROW(handler.handle_single_value({sensor, value, now}));
  return sensor;
}

void check_entry(const wolf::sensor_value &entry, const wolf::sensor_id &sensor,
                 const float value) {
  EXPECT_EQ(entry.id, sensor);
  EXPECT_EQ(entry.value, value);
}

void test_update_sensor(wolf::sensor_value_handler &handler,
                        const wolf::sensor_type &sensor_type,
                        const wolf::types::id_esp3 &id, const float value,
                        int count, unsigned int index, const bool humidity) {
  auto sensor = test_handle(handler, sensor_type, id, value, humidity);
  wolf::sensor_value_handler::entries_type entries;
  EXPECT_NO_THROW(entries = handler.get_all());
  EXPECT_EQ(entries.size(), count);
  check_entry(entries[index], sensor, value);
}

void test_get_all(wolf::sensor_value_handler &handler,
                  const wolf::sensor_type &sensor_type,
                  const wolf::types::id_esp3 &id, const float value, int &count,
                  const bool humidity) {
  auto index = static_cast<unsigned int>(count - 1);
  test_update_sensor(handler, sensor_type, id, value, count, index, humidity);
  ++count;
}

void test_get_value(wolf::sensor_value_handler &handler,
                    const wolf::sensor_type &sensor_type,
                    const wolf::types::id_esp3 &id, const float value,
                    const bool humidity) {
  auto sensor = test_handle(handler, sensor_type, id, value, humidity);
  const auto check_value = handler.get_value_float(sensor);
  EXPECT_TRUE(check_value);
  EXPECT_EQ(check_value, value);
}

void test_get_value_with_offset_sht25(
    wolf::sensor_value_offset_handler &offset_handler,
    wolf::sensor_value_handler &handler, const float value, const float offset,
    const wolf::sensor_sht25_channel &channel) {
  auto sensor = create_sensor_id_sht25(channel);
  float offset_humidity = offset;
  float offset_temperature = 2 * offset_humidity;
  offset_handler.set({sensor, offset_humidity});
  if (channel == wolf::sensor_sht25_channel::temperature)
    offset_handler.set({sensor, offset_temperature});
  const auto now = std::chrono::system_clock::now();
  EXPECT_NO_THROW(handler.handle_single_value({sensor, value, now}));
  const auto check_value = handler.get_value_float(sensor);
  EXPECT_TRUE(check_value);
  auto value_with_offset = value + offset_humidity;
  if (channel == wolf::sensor_sht25_channel::temperature)
    value_with_offset = value + offset_temperature;
  EXPECT_EQ(check_value, value_with_offset);
}

void test_get_value_with_offset_enocean(
    wolf::sensor_value_offset_handler &offset_handler,
    wolf::sensor_value_handler &handler, const wolf::types::id_esp3 &id,
    const float value, const float offset, const CHANNEL_TYPE &channel) {
  auto sensor = create_sensor_id_enocean(id, channel);
  float offset_humidity = offset;
  float offset_temperature = 2 * offset_humidity;
  const auto id_temperature = create_sensor_id_enocean(id, S_TEMP);
  offset_handler.set({id_temperature, offset_temperature});
  const auto id_humidity = create_sensor_id_enocean(id, S_RELHUM);
  offset_handler.set({id_humidity, offset_humidity});
  const auto now = std::chrono::system_clock::now();
  EXPECT_NO_THROW(handler.handle_single_value({sensor, value, now}));
  const auto check_value = handler.get_value_float(sensor);
  EXPECT_TRUE(check_value);
  auto value_with_offset = value + offset_humidity;
  if (channel == S_TEMP) value_with_offset = value + offset_temperature;
  EXPECT_EQ(check_value, value_with_offset);
}

struct test_value_handler_bundle {
  boost::asio::io_service service;
  database_in_memory database;
  wolf::sensor_value_offset_cache sensor_value_offset_cache{database};
  wolf::sensor_value_offset_handler value_offset_handler{
      sensor_value_offset_cache};
  wolf::sensor_value_handler handler{
      service,
      value_offset_handler,
  };
};

TEST(TestSensorValueHandler, HandleNoError) {
  test_value_handler_bundle to_test;
  test_handle(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, true);
}

TEST(TestSensorValueHandler, HandleMultibleSensors) {
  test_value_handler_bundle to_test;
  test_handle(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, true);
  test_handle(to_test.handler, wolf::sensor_type::enocean, 2, 10.f, true);
  test_handle(to_test.handler, wolf::sensor_type::sht25, 3, 13.5f, true);
}

TEST(TestSensorValueHandler, GetAllNoError) {
  test_value_handler_bundle to_test;
  auto count = 1;
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, count, true);
}

TEST(TestSensorValueHandler, GetAllMoreSensors) {
  test_value_handler_bundle to_test;
  auto count = 1;
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, count, true);
  test_get_all(to_test.handler, wolf::sensor_type::enocean, 2, 10.f, count,
               true);
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 3, 13.5f, count,
               false);
}

TEST(TestSensorValueHandler, UpdateSensor) {
  test_value_handler_bundle to_test;
  auto count = 1;
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, count, true);
  auto count_update = 1;
  test_update_sensor(to_test.handler, wolf::sensor_type::sht25, 1, 25.4f,
                     count_update, 0, true);
}

TEST(TestSensorValueHandler, UpdateMoreSensors) {
  test_value_handler_bundle to_test;
  auto count = 1;
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, count, true);
  test_get_all(to_test.handler, wolf::sensor_type::enocean, 2, 10.f, count,
               true);
  test_get_all(to_test.handler, wolf::sensor_type::sht25, 3, 13.5f, count,
               false);
  auto count_update = 3;
  test_update_sensor(to_test.handler, wolf::sensor_type::sht25, 1, 25.4f,
                     count_update, 0, true);
  test_update_sensor(to_test.handler, wolf::sensor_type::enocean, 2, 4.3f,
                     count_update, 1, true);
  test_update_sensor(to_test.handler, wolf::sensor_type::sht25, 3, 12.7f,
                     count_update, 2, false);
}

TEST(TestSensorValueHandler, GetValueNoError) {
  test_value_handler_bundle to_test;
  test_get_value(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, true);
}

TEST(TestSensorValueHandler, GetValueSensorIDNotValid) {
  test_value_handler_bundle to_test;
  auto sensor =
      test_handle(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, true);
  auto wrong_id =
      create_sensor_id_sht25(wolf::sensor_sht25_channel::temperature);
  EXPECT_FALSE(to_test.handler.get_value_float(wrong_id).is_initialized());
}

TEST(TestSensorValueHandler, GetValueMoreSensors) {
  test_value_handler_bundle to_test;
  test_get_value(to_test.handler, wolf::sensor_type::sht25, 1, 20.f, true);
  test_get_value(to_test.handler, wolf::sensor_type::enocean, 2, 10.f, true);
  test_get_value(to_test.handler, wolf::sensor_type::sht25, 3, 13.5f, true);
}

TEST(TestSensorValueHandler, HandleValueInvalidTimestamp) {
  test_value_handler_bundle to_test;
  auto sensor = create_sensor_id_sht25(wolf::sensor_sht25_channel::temperature);
  // Thursday, 10. September 2015 15:56:29
  std::chrono::milliseconds time_point_in_millis(1441900589000);
  time_point invalid_time_point(time_point_in_millis);
  EXPECT_NO_THROW(
      to_test.handler.handle_single_value({sensor, 20.f, invalid_time_point}));
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumiditySHT25) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumiditySHT25UpdateValue) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 22.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 19.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumiditySHT25UpdateOffset) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.7f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.3f,
                                   wolf::sensor_sht25_channel::humidity);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureSHT25) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureSHT25UpdateValue) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 22.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 19.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureSHT25UpdateOffset) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.7f,
                                   wolf::sensor_sht25_channel::temperature);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.3f,
                                   wolf::sensor_sht25_channel::temperature);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumidityAndTemperatureSHT25) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 20.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 25.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumidityEnocean) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_RELHUM);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumidityEnoceanUpdateValue) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_RELHUM);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 22.f, 0.5f, S_RELHUM);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 19.f, 0.5f, S_RELHUM);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumidityEnoceanUpdateOffset) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_RELHUM);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.7f, S_RELHUM);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.3f, S_RELHUM);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureEnocean) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_TEMP);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureEnoceanUpdateValue) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_TEMP);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 22.f, 0.5f, S_TEMP);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 19.f, 0.5f, S_TEMP);
}

TEST(TestSensorValueHandler, GetValueWithOffsetTemperatureEnoceanUpdateOffset) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_TEMP);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.7f, S_TEMP);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.3f, S_TEMP);
}

TEST(TestSensorValueHandler, GetValueWithOffsetHumidityAndTemperatureEnocean) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 20.f, 0.5f, S_RELHUM);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 2, 25.f, 0.5f, S_TEMP);
}

TEST(TestSensorValueHandler,
     GetValueWithOffsetHumidityAndTemperatureSHT25AndEnocean) {
  test_value_handler_bundle to_test;
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 1, 40.f, 0.5f, S_RELHUM);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 30.f, 0.5f,
                                   wolf::sensor_sht25_channel::humidity);
  test_get_value_with_offset_enocean(to_test.value_offset_handler,
                                     to_test.handler, 3, 20.f, 0.5f, S_TEMP);
  test_get_value_with_offset_sht25(to_test.value_offset_handler,
                                   to_test.handler, 15.f, 0.5f,
                                   wolf::sensor_sht25_channel::temperature);
}
}  // namespace sensor_value_handler_test
