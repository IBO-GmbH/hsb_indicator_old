#include "mold/configuration_state_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/di/extension/scopes/shared.hpp>
#include "create_room.hpp"
#include "create_shared_with_injector.hpp"
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "mold/mold_handler.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "test_sensors.hpp"
#include "wolf/sensor_value_handler.hpp"

class options_mock : public mold::options_handler {
 public:
  options_mock(mold::options_cache& database)
      : mold::options_handler(database, 0.71f, 0.6f) {}

  float get_frsi() const override { return 0.71f; }
};

static auto create_injector(boost::asio::io_context& context) {
  namespace di = boost::di;
  mold::config mold_config;
  mold::configuration_state_handler::config mold_state_handler_config{.05f,
                                                                      .75f};
  return di::make_injector<di::extension::shared_config>(
      di::bind<wolf::database>().to<database_in_memory>(),
      di::bind<boost::asio::io_context>().to(context),
      di::bind<mold::options_handler>().to<options_mock>(),
      di::bind<mold::config>().to(std::move(mold_config)),
      di::bind<mold::configuration_state_handler::config>().to(
          std::move(mold_state_handler_config)));
}

struct ConfigurationStateHandler : testing::Test {
  logging::logger logger{"ConfigurationStateHandler"};
  boost::asio::io_context context;
  decltype(create_injector(context)) injector = create_injector(context);
  wolf::sensor_value_handler& sensor_value_handler =
      injector.create<wolf::sensor_value_handler&>();
  mold::configuration_handler& configuration_handler =
      injector.create<mold::configuration_handler&>();
  mold::mold_handler& mold_handler = injector.create<mold::mold_handler&>();
  mold::configuration_state_handler& configuration_state_handler =
      injector.create<mold::configuration_state_handler&>();
  const test_sensors::indoor indoor_sensors =
      test_sensors::create_indoor_sensors();
  const wolf::outdoor_sensor outdoor_sensor =
      test_sensors::create_outdoor_sensor();
  ConfigurationStateHandler() {
    create_room(injector,
                {indoor_sensors.temperature, indoor_sensors.humidity});
    injector.create<mold::outdoor_sensor_change_helper>().set(outdoor_sensor);
  }
};

TEST_F(ConfigurationStateHandler, SeveralValuesOneByOne) {
  int call_count{};
  configuration_state_handler.signal_state_changed.connect(
      [&](const auto& /*id*/, const auto& /*state*/) { ++call_count; });
  const auto now = std::chrono::system_clock::now();
  sensor_value_handler.handle_single_value(
      {indoor_sensors.temperature, 37.f, now});
  sensor_value_handler.handle_single_value(
      {indoor_sensors.humidity, 80.f, now});
  sensor_value_handler.handle_single_value(
      {outdoor_sensor.temperature, -5.f, now});
  sensor_value_handler.handle_single_value(
      {outdoor_sensor.humidity, 40.f, now});
  EXPECT_EQ(call_count, 0);
  sensor_value_handler.handle_single_value(
      {outdoor_sensor.humidity, 41.f, now});
  EXPECT_EQ(call_count, 0);
}

TEST_F(ConfigurationStateHandler, SeveralValuesAtOnce) {
  int call_count{};
  configuration_state_handler.signal_state_changed.connect(
      [&](const auto& id, const auto& state) {
        ++call_count;
        EXPECT_NO_THROW(configuration_handler.get_or_throw(id));
        LOGGING_LOG(logger, logging::severity::normal) << "state:" << state;
      });
  const auto now = std::chrono::system_clock::now();
  sensor_value_handler.handle_list({{outdoor_sensor.temperature, -5.f, now},
                                    {outdoor_sensor.humidity, 40.f, now}});
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 37.f, now},
                                    {indoor_sensors.humidity, 80.f, now}});
  EXPECT_EQ(call_count, 0);
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 47.f, now},
                                    {indoor_sensors.humidity, 90.f, now}});
  EXPECT_EQ(call_count, 0);
}

TEST_F(ConfigurationStateHandler, SproutValueHandler) {
  auto& sprout_value_handler = injector.create<mold::sprout::handler&>();
  int call_count{};
  sprout_value_handler.signal_sprout_type_changed.connect(
      [&](const auto& sprout_value) {
        ++call_count;
        EXPECT_NO_THROW(
            configuration_handler.get_or_throw(sprout_value.configuration));
      });
  auto now = std::chrono::system_clock::now();
  sensor_value_handler.handle_list({{outdoor_sensor.temperature, 20.f, now},
                                    {outdoor_sensor.humidity, 20.f, now}});
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 40.f, now},
                                    {indoor_sensors.humidity, 90.f, now}});
  now = std::chrono::system_clock::now();
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 45.f, now},
                                    {indoor_sensors.humidity, 80.f, now}});
  EXPECT_EQ(call_count, 1);
}

TEST_F(ConfigurationStateHandler, MoldValueHandler) {
  auto& mold_value_handler = injector.create<mold::mold_value_handler&>();
  int state_call_count{};
  configuration_state_handler.signal_state_changed.connect(
      [&](const auto& /*id*/, const auto& state) {
        ++state_call_count;
        EXPECT_EQ(state, mold::calculation_state::yellow);
      });
  int call_count{};
  bool there_shall_be_chances_of_mold{};
  mold_value_handler.signal_value.connect(
      [&](const mold::mold_value& mold_value) {
        ++call_count;
        EXPECT_NO_THROW(
            configuration_handler.get_or_throw(mold_value.configuration));
        if (!there_shall_be_chances_of_mold) {
          EXPECT_EQ(mold_value.percentage, 0.f);
          return;
        }
        EXPECT_GT(mold_value.percentage, 0.f);
        LOGGING_LOG(logger, logging::severity::normal)
            << "mold_value.percentage:" << mold_value.percentage;
        EXPECT_EQ(mold_value.sprout_type_, mold::sprout_type::one_day);
      });
  // else the timedifference would be so small, that the calculated mold_value
  // is smaller than float epsilon
  auto now = std::chrono::system_clock::now() - std::chrono::seconds(10);
  sensor_value_handler.handle_list({{outdoor_sensor.temperature, -20.f, now},
                                    {outdoor_sensor.humidity, 20.f, now}});
  there_shall_be_chances_of_mold = true;
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 40.f, now},
                                    {indoor_sensors.humidity, 90.f, now}});
  now = std::chrono::system_clock::now() + std::chrono::hours(2);
  sensor_value_handler.handle_list({{indoor_sensors.temperature, 45.f, now},
                                    {indoor_sensors.humidity, 80.f, now}});
  EXPECT_EQ(state_call_count, 1);
  EXPECT_EQ(call_count, 2);
}
