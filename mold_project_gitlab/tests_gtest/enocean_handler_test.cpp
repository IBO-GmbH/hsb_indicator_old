#include "wolf/enocean_handler.hpp"
#include "fixture_value_handler_mock.hpp"
#include "gtest/gtest.h"

std::unique_ptr<wolf::profile_handler> make_profile() {
  std::unique_ptr<wolf::enocean_link> enocean_link;
  enocean_link.reset(new wolf::enocean_link());
  std::unique_ptr<wolf::profile_handler> profile;
  profile.reset(new wolf::profile_handler(
      {enocean_link.get(), nullptr, nullptr, nullptr}));
  return profile;
}

wolf::enocean_link::parse_results create_parse_results(
    std::size_t &count, const float value, const std::string &name,
    const CHANNEL_TYPE &type, const std::string &unit) {
  wolf::enocean_link::parse_results results;
  results.resize(count);
  for (auto &result : results) {
    result.value = value + count;
    result.name = name + std::to_string(count);
    result.type = type;
    result.unit = unit + std::to_string(count);
    --count;
  }
  return results;
}

wolf::sensor_id create_sensor_id(const wolf::sensor_type &type,
                                 const std::uint8_t &id_digit) {
  wolf::types::id id;
  id.resize(8);
  std::fill(id.begin(), id.end(), id_digit);
  wolf::sensor_id sensor(type, id, id, id);
  return sensor;
}

void test_handle_enocean(wolf::enocean_handler &handler,
                         const wolf::sensor_type &sensor_type,
                         const std::uint8_t &id_digit, const std::size_t &count,
                         const float value, const std::string &name,
                         const CHANNEL_TYPE &channel_type,
                         const std::string &unit) {
  auto sensor = create_sensor_id(sensor_type, id_digit);
  std::size_t restults_size = count;
  auto results =
      create_parse_results(restults_size, value, name, channel_type, unit);
  handler.handle_enocean(sensor, results);
}

TEST(TestEnoceanHandler, HandleEnOceanNoError) {
  boost::asio::io_service service;
  fixture_value_handler_mock value_handler(service);
  auto profile = make_profile();
  wolf::enocean_handler handler(value_handler);
  value_handler.set_check_value(11.f);
  test_handle_enocean(handler, wolf::sensor_type::enocean, 1, 1, 10.f, "name",
                      CHANNEL_TYPE::E_VOC, "unit");
}

TEST(TestEnoceanHandler, HandleEnOceanDifferentType) {
  boost::asio::io_service service;
  fixture_value_handler_mock value_handler(service);
  auto profile = make_profile();
  wolf::enocean_handler handler(value_handler);
  auto sensor = create_sensor_id(wolf::sensor_type::sht25, 1);
  wolf::enocean_link::parse_results results;
  results.resize(0);
  EXPECT_THROW(handler.handle_enocean(sensor, results), std::runtime_error);
}

TEST(TestEnoceanHandler, HandleEnOceanNoData) {
  boost::asio::io_service service;
  fixture_value_handler_mock value_handler(service);
  auto profile = make_profile();
  wolf::enocean_handler handler(value_handler);
  auto sensor = create_sensor_id(wolf::sensor_type::enocean, 1);
  wolf::enocean_link::parse_results results;
  results.resize(0);
  handler.handle_enocean(sensor, results);
}

TEST(TestEnoceanHandler, HandleEnOceanMultibleSensors) {
  boost::asio::io_service service;
  fixture_value_handler_mock value_handler(service);
  auto profile = make_profile();
  wolf::enocean_handler handler(value_handler);
  value_handler.set_check_value(11.f);
  test_handle_enocean(handler, wolf::sensor_type::enocean, 1, 1, 10.f, "name 1",
                      CHANNEL_TYPE::E_VOC, "unit 1");
  value_handler.set_check_value(13.3f);
  test_handle_enocean(handler, wolf::sensor_type::enocean, 2, 1, 12.3f,
                      "name 2", CHANNEL_TYPE::E_DAYS, "unit 2");
  value_handler.set_check_value(1.5f);
  test_handle_enocean(handler, wolf::sensor_type::enocean, 3, 1, 0.5f, "name 3",
                      CHANNEL_TYPE::E_FANSPEED, "unit 3");
}
