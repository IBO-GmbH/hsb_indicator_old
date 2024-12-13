#include <boost/di.hpp>
#include <boost/di/extension/scopes/shared.hpp>
#include "create_room.hpp"
#include "create_shared_with_injector.hpp"
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "mold/mold_handler.hpp"
#include "mold/room_creator.hpp"
#include "wolf/sensor_id_scd30.hpp"
#include "wolf/sensor_id_shtc1.hpp"

class options_mock : public mold::options_handler {
 public:
  options_mock(mold::options_cache& database)
      : mold::options_handler(database, 0.71f, 0.6f) {}

  float get_frsi() const override { return 0.71f; }
};

static auto make_config() {
  mold::config mold_config;
  mold_config.evaluation_humidity_.filter_constant = 0.25f;
  mold_config.evaluation_humidity_.filter_threshold = 2.0f;
  mold_config.evaluation_humidity_.filter_threshold_with_time = 2.0f * 600.f;
  mold_config.evaluation_humidity_.filter_threshold_timeout = 15 * 60;
  return mold_config;
}

static auto create_injector(boost::asio::io_context& context) {
  namespace di = boost::di;
  auto mold_config = make_config();
  auto injector = di::make_injector<di::extension::shared_config>(
      di::bind<wolf::database>().to<database_in_memory>(),
      di::bind<boost::asio::io_context>().to(context),
      di::bind<mold::config>().to(std::move(mold_config)),
      di::bind<mold::options_handler>().to<options_mock>());
  return injector;
}

template <class create_type, class creator_type>
static std::shared_ptr<create_type> create_shared(creator_type& injector) {
  return create_shared_with_injector<create_type>(injector);
}

namespace {
struct EvaluationHumidity : public ::testing::Test {
  logging::logger logger{"EvaluationHumidity"};
  boost::asio::io_context context;
  decltype(create_injector(context)) injector = create_injector(context);
  std::shared_ptr<wolf::sensor_value_handler> sensor_value_handler =
      create_shared<wolf::sensor_value_handler>(injector);
  std::shared_ptr<mold::evaluation_humidity::handler>
      evaluation_humidity_handler =
          create_shared<mold::evaluation_humidity::handler>(injector);
  std::shared_ptr<mold::mold_handler> mold_handler =
      create_shared<mold::mold_handler>(injector);

  const std::chrono::system_clock::time_point now =
      std::chrono::system_clock::now();
  wolf::sensor_id sensor_id_indoor_temperature =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::temperature);
  wolf::sensor_id sensor_id_indoor_humidity =
      wolf::sensor_id_shtc1::create(wolf::sensor_shtc1_channel::humidity);
  const wolf::sensor_value value_tempature{sensor_id_indoor_temperature, 25.f,
                                           now};
  const wolf::sensor_value value_humidity{sensor_id_indoor_humidity, 35.f, now};
  const wolf::sensor_value value_tempature_2{sensor_id_indoor_temperature,
                                             24.5f, now};
  const wolf::sensor_value value_humidity_2{sensor_id_indoor_humidity, 80.f,
                                            now};
  const wolf::sensor_value value_tempature_3{sensor_id_indoor_temperature, 24.f,
                                             now};
  const wolf::sensor_value value_humidity_3{sensor_id_indoor_humidity, 40.f,
                                            now};
};
}  // namespace

template <class injector_type>
static void set_outdoor(injector_type& injector,
                        const wolf::outdoor_sensor& outdoor_sensor) {
  auto configuration_handler =
      create_shared<mold::configuration_handler>(injector);
  configuration_handler->set_outdoor_sensor(outdoor_sensor);
}

static auto create_outdoor_sensor() {
  wolf::outdoor_sensor result;
  result.temperature =
      wolf::sensor_id_scd30::create(wolf::sensor_scd30_channel::temperature);
  return result;
}
static void create_and_set_outdoor_values(EvaluationHumidity& test) {
  const auto outdoor = create_outdoor_sensor();
  set_outdoor(test.injector, outdoor);
  const wolf::sensor_value value_outdoor_tempature{outdoor.temperature, -10.f,
                                                   test.now};
  test.sensor_value_handler->handle_single_value(value_outdoor_tempature);
}

TEST_F(EvaluationHumidity, OnlyOneCalculationOneValueOnlyMedianChange) {
  auto room_config = create_room(
      injector, {sensor_id_indoor_temperature, sensor_id_indoor_humidity});
  int call_count_median{};
  evaluation_humidity_handler->signal_median.connect([&](auto id, auto) {
    EXPECT_EQ(id, room_config.configuration_.id);
    ++call_count_median;
  });
  int call_count_filter{};
  evaluation_humidity_handler->signal_filtered.connect(
      [&](auto) { ++call_count_filter; });
  create_and_set_outdoor_values(*this);
  sensor_value_handler->handle_single_value(value_humidity);
  EXPECT_EQ(call_count_median, 0);
  EXPECT_EQ(call_count_filter, 0);
  sensor_value_handler->handle_single_value(value_tempature);
  EXPECT_EQ(call_count_median, 1);
  EXPECT_EQ(call_count_filter, 1);
  sensor_value_handler->handle_single_value(value_tempature_2);
  EXPECT_EQ(call_count_median, 2);
  EXPECT_EQ(call_count_filter, 1);
  sensor_value_handler->handle_single_value(value_tempature_3);
  EXPECT_EQ(call_count_median, 3);
  EXPECT_EQ(call_count_filter, 1);
}

TEST_F(EvaluationHumidity, OnlyOneCalculationOneValueFilterChange) {
  auto room_config = create_room(
      injector, {sensor_id_indoor_temperature, sensor_id_indoor_humidity});
  int call_count_median{};
  evaluation_humidity_handler->signal_median.connect([&](auto id, auto) {
    EXPECT_EQ(id, room_config.configuration_.id);
    ++call_count_median;
  });
  int call_count_filter{};
  evaluation_humidity_handler->signal_filtered.connect(
      [&](auto) { ++call_count_filter; });
  create_and_set_outdoor_values(*this);
  sensor_value_handler->handle_single_value(value_humidity);
  EXPECT_EQ(call_count_median, 0);
  EXPECT_EQ(call_count_filter, 0);
  sensor_value_handler->handle_single_value(value_tempature);
  EXPECT_EQ(call_count_median, 1);
  EXPECT_EQ(call_count_filter, 1);
  sensor_value_handler->handle_single_value(value_tempature_2);
  EXPECT_EQ(call_count_median, 2);
  EXPECT_EQ(call_count_filter, 1);
  sensor_value_handler->handle_single_value(value_humidity_2);
  EXPECT_EQ(call_count_median, 3);
  EXPECT_EQ(call_count_filter, 2);
}

TEST_F(EvaluationHumidity, OnlyOneCalculationTwoValuesOnlyMedianChange) {
  auto room_config = create_room(
      injector, {sensor_id_indoor_temperature, sensor_id_indoor_humidity});
  int call_count_median{};
  evaluation_humidity_handler->signal_median.connect([&](auto id, auto) {
    EXPECT_EQ(id, room_config.configuration_.id);
    ++call_count_median;
  });
  int call_count_filter{};
  evaluation_humidity_handler->signal_filtered.connect(
      [&](auto) { ++call_count_filter; });
  create_and_set_outdoor_values(*this);
  EXPECT_EQ(call_count_median, 0);
  EXPECT_EQ(call_count_filter, 0);
  std::vector<wolf::sensor_value> values{value_humidity, value_tempature};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 1);
  EXPECT_EQ(call_count_filter, 1);
  values = {value_humidity, value_tempature_2};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 2);
  EXPECT_EQ(call_count_filter, 1);
  values = {value_humidity, value_tempature_3};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 3);
  EXPECT_EQ(call_count_filter, 1);
}

TEST_F(EvaluationHumidity, OnlyOneCalculationTwoValuesFilterChange) {
  auto room_config = create_room(
      injector, {sensor_id_indoor_temperature, sensor_id_indoor_humidity});
  int call_count_median{};
  evaluation_humidity_handler->signal_median.connect([&](auto id, auto) {
    EXPECT_EQ(id, room_config.configuration_.id);
    ++call_count_median;
  });
  int call_count_filter{};
  evaluation_humidity_handler->signal_filtered.connect(
      [&](auto) { ++call_count_filter; });
  create_and_set_outdoor_values(*this);
  EXPECT_EQ(call_count_median, 0);
  EXPECT_EQ(call_count_filter, 0);
  std::vector<wolf::sensor_value> values{value_humidity, value_tempature};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 1);
  EXPECT_EQ(call_count_filter, 1);
  values = {value_humidity_2, value_tempature_2};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 2);
  EXPECT_EQ(call_count_filter, 2);
  values = {value_humidity_3, value_tempature_3};
  sensor_value_handler->handle_list(values);
  EXPECT_EQ(call_count_median, 3);
  EXPECT_EQ(call_count_filter, 3);
}
