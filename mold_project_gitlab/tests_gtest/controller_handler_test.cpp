#include "wolf/controller/controller_handler.hpp"
#include "fixture_value_handler_mock.hpp"
#include "gtest/gtest.h"
#include "wolf/database.hpp"
#include "wolf/sensor_id_enocean.hpp"

namespace controller_handler_test {

using id_array_3 = std::array<wolf::types::uuid_array, 3>;
using id_array_6 = std::array<wolf::types::uuid_array, 6>;
using room_array = std::array<wolf::room, 6>;

static logging::logger test_logger("controller_handler_test");
static wolf::controller::controller_cache::values_list values;

class controller_cache_mock : public wolf::controller::controller_cache {
 public:
  controller_cache_mock(wolf::database &database_)
      : wolf::controller::controller_cache(database_) {}
  bool add(const wolf::controller::values_per_room &to_add) override {
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "added " << to_add << " to database";
    return true;
  }

  bool update(const wolf::controller::values_per_room &to_update) override {
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "updated " << to_update << " in database";
    return true;
  }

  bool remove(const wolf::types::uuid_array &room_id) override {
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "removed " << room_id << " from database";
    return true;
  }

  virtual values_list get_all() override { return values; }
};

struct controller_handler_test {
  wolf::database new_database{":memory:"};
  wolf::database_transaction_helper transaction_helper{new_database};
  boost::asio::io_service service;
  std::unique_ptr<controller_cache_mock> new_cache;
  std::shared_ptr<wolf::controller::controller_handler> new_handler;
  wolf::program_options_parser program_options;

  controller_handler_test(fixture_value_handler_mock &values_handler) {
    bool end_programm = false;
    std::vector<char> program_path_mock{'t', 'e', 's', 't'};
    std::vector<char *> argv;
    argv.push_back(program_path_mock.data());
    program_options.parse(argv.size(), argv.data(), "test", end_programm);
    new_cache.reset(new controller_cache_mock(new_database));
    new_handler.reset(new wolf::controller::controller_handler(
        values_handler, new_cache.get(), program_options.get_config()));
  }

  ~controller_handler_test() { new_database.close(); }
};

wolf::types::uuid_array make_room_id(const unsigned int digit) {
  wolf::types::uuid_array id;
  std::fill(id.begin(), id.end(), digit);
  return id;
}

wolf::types::uuid_array add_room(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const unsigned int digit) {
  auto id = make_room_id(digit);
  EXPECT_TRUE(new_handler->add(id));
  return id;
}

void add_more_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_3 &ids) {
  unsigned int count = 1;
  for (auto &id : ids) {
    id = add_room(new_handler, count);
    ++count;
  }
}

void check_room_id(wolf::controller::controller_per_room &values,
                   const unsigned int digit) {
  EXPECT_EQ(values.room_id, make_room_id(digit));
}

void check_standard_limits(wolf::controller::controller_per_room &values) {
  wolf::controller::limits standard;
  EXPECT_EQ(values.limits_, standard);
}

void log_room_values(wolf::controller::controller_per_room &values) {
  LOGGING_LOG(test_logger, logging::severity::normal)
      << "room_id: " << values.room_id
      << ", controller: " << *values.controller_
      << ", limits: " << values.limits_;
}

wolf::controller::limits make_new_limits(const float change) {
  wolf::controller::limits new_limits;
  new_limits.humidity.max = 50.f + change;
  new_limits.humidity.drH = 3.f + change;
  new_limits.humidity_border = 80.f + change;
  new_limits.temperature_.min = 17.f + change;
  new_limits.temperature_.reference = 22.f + change;
  new_limits.air_quality_.co2 = 1000.f + change;
  new_limits.air_quality_.sprout_value = 50.f + change;
  new_limits.air_quality_.voc = 200.f + change;
  LOGGING_LOG(test_logger, logging::severity::normal)
      << "limits: " << new_limits;
  return new_limits;
}

wolf::controller::limits set_limits(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, const float change) {
  auto new_limits = make_new_limits(change);
  EXPECT_TRUE(new_handler->set_limits(id, new_limits));
  return new_limits;
}

wolf::controller::limits get_limits(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id) {
  wolf::controller::limits limits_get;
  EXPECT_NO_THROW(limits_get = new_handler->get_limits(id));
  return limits_get;
}

void check_limits_set(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, const unsigned int faktor) {
  float change = 0.05f * faktor / 10;
  auto new_limits = set_limits(new_handler, id, change);
  auto limits_get = get_limits(new_handler, id);
  EXPECT_EQ(limits_get, new_limits);
}

void check_limits_more_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_3 &ids) {
  add_more_rooms(new_handler, ids);
  unsigned int count = 1;
  for (auto &id : ids) {
    check_limits_set(new_handler, id, count);
    ++count;
  }
}

void add_values() {
  values.resize(3);
  unsigned int count = 1;
  for (auto &value : values) {
    value.room_id = make_room_id(count);
    value.limits_ = make_new_limits(static_cast<float>(count) / 10);
    value.actors = {};
    value.active_loops = {{true, false, true, false}};
    ++count;
  }
}

static wolf::sensor_id create_enocean_sensor_id(const std::uint8_t &id_digit,
                                                const CHANNEL_TYPE &channel) {
  return wolf::sensor_id_enocean::create(
      id_digit, channel, std::optional<wolf::enocean_subchannel_type>(), {});
}

wolf::outdoor_sensor make_outdoor() {
  auto humidity = create_enocean_sensor_id(1, S_RELHUM);
  auto temperature = create_enocean_sensor_id(2, S_TEMP);
  wolf::outdoor_sensor outdoor(temperature, humidity);
  return outdoor;
}

void set_outdoor(
    std::shared_ptr<wolf::controller::controller_handler> new_handler) {
  auto outdoor = make_outdoor();
  EXPECT_NO_THROW(new_handler->set_outdoor_sensor(outdoor));
}

wolf::controller::controller_handler::active_states make_states(
    unsigned int count) {
  wolf::controller::controller_handler::active_states states{};
  for (auto index = count; index < states.size(); ++index) {
    states[index] = true;
  }
  for (auto &state : states)
    LOGGING_LOG(test_logger, logging::severity::normal) << "state: " << state;
  return states;
}

wolf::controller::controller_handler::active_states set_active_states(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, unsigned int count) {
  auto states = make_states(count);
  EXPECT_TRUE(new_handler->set_active_states(id, states));
  return states;
}

wolf::controller::controller_handler::active_states get_active_states(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id) {
  wolf::controller::controller_handler::active_states states;
  EXPECT_NO_THROW(states = new_handler->get_active_states(id));
  return states;
}

void check_active_states(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, unsigned int count) {
  auto states_set = set_active_states(new_handler, id, count);
  auto states_get = get_active_states(new_handler, id);
  EXPECT_EQ(states_get, states_set);
}

void check_active_states_more_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_3 &ids) {
  add_more_rooms(new_handler, ids);
  unsigned int count = 1;
  for (auto &id : ids) {
    check_active_states(new_handler, id, count);
    ++count;
  }
}

wolf::actor::actor make_actor(const unsigned int digit) {
  return {digit, false};
}

wolf::actor::actor set_actor(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, const unsigned int digit,
    const unsigned int index) {
  auto new_actor = make_actor(digit);
  EXPECT_TRUE(new_handler->add_actor(id, new_actor, index));
  return new_actor;
}

void add_one_actor_per_room(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_3 &ids) {
  add_more_rooms(new_handler, ids);
  unsigned int count = 1;
  for (auto &id : ids) {
    set_actor(new_handler, id, count, 1);
    ++count;
  }
}

void add_three_actors_per_room(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_3 &ids) {
  add_more_rooms(new_handler, ids);
  unsigned int count = 1;
  for (auto &id : ids) {
    set_actor(new_handler, id, 1, 1);
    set_actor(new_handler, id, 2, 2);
    set_actor(new_handler, id, 3, 3);
    ++count;
  }
}

void check_get_actors_one_actor(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, wolf::actor::actor &new_actor) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  auto max_actors = wolf::controller::controller::actors_per_room;
  EXPECT_EQ(actors.size(), max_actors);
  EXPECT_EQ(actors[0], boost::none);
  EXPECT_EQ(actors[1], new_actor);
  for (unsigned int index = 2; index < 16; ++index)
    EXPECT_EQ(actors[index], boost::none);
}

void check_get_actors_three_actors(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    const wolf::types::uuid_array &id, wolf::actor::actor &new_actor1,
    wolf::actor::actor &new_actor2, wolf::actor::actor &new_actor3) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  auto max_actors = wolf::controller::controller::actors_per_room;
  EXPECT_EQ(actors.size(), max_actors);
  EXPECT_EQ(actors[0], boost::none);
  EXPECT_EQ(actors[1], new_actor1);
  EXPECT_EQ(actors[2], new_actor2);
  EXPECT_EQ(actors[3], new_actor3);
  for (unsigned int index = 4; index < 16; ++index)
    EXPECT_EQ(actors[index], boost::none);
}

wolf::room make_room(wolf::types::uuid_array &id) {
  wolf::room room;
  room.id = id;
  room.name = "name";
  auto humidity = create_enocean_sensor_id(1, S_RELHUM);
  room.sensors.push_back(humidity);
  auto temperature = create_enocean_sensor_id(2, S_TEMP);
  room.sensors.push_back(temperature);
  auto co2 = create_enocean_sensor_id(3, S_CONC);
  room.sensors.push_back(co2);
  auto voc = create_enocean_sensor_id(4, E_VOC);
  room.sensors.push_back(voc);
  auto sprout_value = create_enocean_sensor_id(5, S_PERCENTAGE);
  room.sensors.push_back(sprout_value);
  return room;
}

void add_all_actors(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id, const unsigned int offset) {
  unsigned int max_actors = wolf::controller::controller::actors_per_room;
  for (unsigned int index = 0; index < max_actors; ++index)
    new_handler->add_actor(id, {index + offset, false}, index);
}

void check_no_actors_set(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  for (unsigned int index = 0; index < 16; ++index)
    EXPECT_FALSE(actors[index].get().state);
}

void check_actors_svas1(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  for (unsigned int index = 0; index < 4; ++index)
    EXPECT_TRUE(actors[index].get().state);
}

void check_actors_svas2_ventilation(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  EXPECT_TRUE(actors[4].get().state);
  EXPECT_FALSE(actors[5].get().state);
  EXPECT_TRUE(actors[6].get().state);
  EXPECT_TRUE(actors[7].get().state);
}

void check_actors_svas2_dehumidifier(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  EXPECT_FALSE(actors[4].get().state);
  EXPECT_TRUE(actors[5].get().state);
  EXPECT_FALSE(actors[6].get().state);
  EXPECT_FALSE(actors[7].get().state);
}

void check_actors_svas3_ventilation(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  EXPECT_TRUE(actors[8].get().state);
  EXPECT_FALSE(actors[9].get().state);
  EXPECT_TRUE(actors[10].get().state);
  EXPECT_TRUE(actors[11].get().state);
}

void check_actors_svas3_heating(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  EXPECT_FALSE(actors[8].get().state);
  EXPECT_TRUE(actors[9].get().state);
  EXPECT_FALSE(actors[10].get().state);
  EXPECT_FALSE(actors[11].get().state);
}

void check_actors_svas4(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  for (unsigned int index = 12; index < 16; ++index)
    EXPECT_TRUE(actors[index].get().state);
}

room_array make_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_6 &ids) {
  room_array rooms;
  unsigned int count = 0;
  for (auto &id : ids) {
    id = add_room(new_handler, count + 1);
    rooms[count] = make_room(id);
    ++count;
  }
  return rooms;
}

void set_active_states_more_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_6 &ids) {
  new_handler->set_active_states(ids[0], {{true, false, false, false}});
  new_handler->set_active_states(ids[1], {{false, true, false, false}});
  new_handler->set_active_states(ids[2], {{false, true, false, false}});
  new_handler->set_active_states(ids[3], {{false, false, true, false}});
  new_handler->set_active_states(ids[4], {{false, false, true, false}});
  new_handler->set_active_states(ids[5], {{false, false, false, true}});
}

void set_values(fixture_value_handler_mock &values_handler) {
  values_handler.set_humidity(65.f);
  values_handler.set_temperature(30.7f);
  values_handler.set_co2(1500.f);
  values_handler.set_sprout_value(70.f);
  values_handler.set_voc(300.f);
}

void add_all_actors_more_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_6 &ids) {
  unsigned int count = 0;
  for (auto &id : ids) {
    auto offset = count * 16;
    add_all_actors(new_handler, id, offset);
    ++count;
  }
}

void control_all_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    room_array &rooms, fixture_value_handler_mock &values_handler) {
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[0], false));
  values_handler.set_humidity(65.f);
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[1], false));
  values_handler.set_humidity(75.f);
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[2], false));
  values_handler.set_humidity(65.f);
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[3], false));
  values_handler.set_temperature(16.f);
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[4], false));
  values_handler.set_temperature(30.7f);
  EXPECT_TRUE(new_handler->handle_control_loops(rooms[5], false));
}

void check_all_rooms(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_6 &ids) {
  check_actors_svas1(new_handler, ids[0]);
  check_actors_svas2_ventilation(new_handler, ids[1]);
  check_actors_svas2_dehumidifier(new_handler, ids[2]);
  check_actors_svas3_ventilation(new_handler, ids[3]);
  check_actors_svas3_heating(new_handler, ids[4]);
  check_actors_svas4(new_handler, ids[5]);
}

void check_actors_no_set(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    wolf::types::uuid_array &id) {
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = new_handler->get_actors_list(id));
  for (unsigned int index = 0; index < 16; ++index)
    EXPECT_FALSE(actors[index].get().state);
}

void check_all_rooms_no_outdoor(
    std::shared_ptr<wolf::controller::controller_handler> new_handler,
    id_array_6 &ids) {
  check_actors_no_set(new_handler, ids[0]);
  check_actors_no_set(new_handler, ids[1]);
  check_actors_svas2_dehumidifier(new_handler, ids[2]);
  check_actors_no_set(new_handler, ids[3]);
  check_actors_svas3_heating(new_handler, ids[4]);
  check_actors_no_set(new_handler, ids[5]);
}

TEST(TestControllerHandler, AddValuesNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  add_room(test.new_handler, 1);
}

TEST(TestControllerHandler, AddValuesValuesLoaded) {
  add_values();
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  EXPECT_FALSE(test.new_handler->add(id));
  values.clear();
}

TEST(TestControllerHandler, AddValuesAlreadyAdded) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  EXPECT_TRUE(test.new_handler->add(id));
  EXPECT_FALSE(test.new_handler->add(id));
}

TEST(TestControllerHandler, AddAndGetValues) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  add_room(test.new_handler, 1);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 1);
  check_room_id(values_list[0], 1);
  check_standard_limits(values_list[0]);
  log_room_values(values_list[0]);
}

TEST(TestControllerHandler, AddAndGetValuesMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  for (unsigned int index = 1; index < 4; ++index)
    add_room(test.new_handler, index);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 3);
  unsigned int count = 1;
  for (auto &values : values_list) {
    check_room_id(values, count);
    check_standard_limits(values);
    log_room_values(values);
    ++count;
  }
}

TEST(TestControllerHandler, UpdateValuesNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  EXPECT_TRUE(test.new_handler->update(id));
}

TEST(TestControllerHandler, UpdateValuesNotAdded) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  EXPECT_FALSE(test.new_handler->update(id));
}

TEST(TestControllerHandler, UpdateValuesNoErrorMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id1 = add_room(test.new_handler, 1);
  EXPECT_TRUE(test.new_handler->update(id1));
  auto id2 = add_room(test.new_handler, 2);
  EXPECT_TRUE(test.new_handler->update(id2));
  auto id3 = add_room(test.new_handler, 3);
  EXPECT_TRUE(test.new_handler->update(id3));
}

TEST(TestControllerHandler, RemoveValuesNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  EXPECT_TRUE(test.new_handler->remove(id));
}

TEST(TestControllerHandler, RemoveValuesNotAdded) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  EXPECT_FALSE(test.new_handler->remove(id));
}

TEST(TestControllerHandler, RemoveAndGetValues) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 1);
  EXPECT_TRUE(test.new_handler->remove(id));
  values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 0);
}

TEST(TestControllerHandler, RemoveAndGetValuesMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  add_room(test.new_handler, 1);
  auto id2 = add_room(test.new_handler, 2);
  add_room(test.new_handler, 3);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 3);
  unsigned int count = 1;
  for (auto &values : values_list) {
    check_room_id(values, count);
    log_room_values(values);
    ++count;
  }
  EXPECT_TRUE(test.new_handler->remove(id2));
  values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 2);
  check_room_id(values_list[0], 1);
  log_room_values(values_list[0]);
  check_room_id(values_list[1], 3);
  log_room_values(values_list[1]);
}

TEST(TestControllerHandler, SetLimitsErrorNoOutdoorSensor) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_limits = set_limits(test.new_handler, id, 0.05f);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 1);
  EXPECT_EQ(values_list[0].limits_, new_limits);
}

TEST(TestControllerHandler, SetLimitsNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_limits = set_limits(test.new_handler, id, 0.05f);
  auto values_list = test.new_handler->get_all();
  EXPECT_EQ(values_list.size(), 1);
  EXPECT_EQ(values_list[0].limits_, new_limits);
}

TEST(TestControllerHandler, GetLimitsStandardValues) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  wolf::controller::limits standard;
  auto limits_get = get_limits(test.new_handler, id);
  EXPECT_EQ(limits_get, standard);
}

TEST(TestControllerHandler, SetAndGetLimits) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  check_limits_set(test.new_handler, id, 1);
}

TEST(TestControllerHandler, SetAndGetLimitsMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  id_array_3 ids;
  check_limits_more_rooms(test.new_handler, ids);
}

TEST(TestControllerHandler, SetAndGetActiveStates) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  check_active_states(test.new_handler, id, 0);
}

TEST(TestControllerHandler, SetAndGetActiveStatesMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  check_active_states_more_rooms(test.new_handler, ids);
}

TEST(TestControllerHandler, AddActorNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  set_actor(test.new_handler, id, 1, 1);
}

TEST(TestControllerHandler, AddActorNoRoom) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  auto new_actor = make_actor(1);
  EXPECT_FALSE(test.new_handler->add_actor(id, new_actor, 1));
}

TEST(TestControllerHandler, AddMoreActors) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  set_actor(test.new_handler, id, 1, 1);
  set_actor(test.new_handler, id, 1, 2);
  set_actor(test.new_handler, id, 1, 3);
}

TEST(TestControllerHandler, OverwriteActor) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  set_actor(test.new_handler, id, 1, 1);
  set_actor(test.new_handler, id, 2, 1);
}

TEST(TestControllerHandler, AddActorMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  add_one_actor_per_room(test.new_handler, ids);
}

TEST(TestControllerHandler, AddMoreActorMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  add_three_actors_per_room(test.new_handler, ids);
}

TEST(TestControllerHandler, RemoveActorsNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  set_actor(test.new_handler, id, 1, 1);
  EXPECT_TRUE(test.new_handler->remove_actors(id, {1, 1}));
}

TEST(TestControllerHandler, RemoveActorsNoAdded) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  EXPECT_TRUE(test.new_handler->remove_actors(id, {0, 15}));
}

TEST(TestControllerHandler, RemoveActorsNoRoom) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = make_room_id(1);
  EXPECT_FALSE(test.new_handler->remove_actors(id, {1, 1}));
}

TEST(TestControllerHandler, RemoveActorsMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  add_one_actor_per_room(test.new_handler, ids);
  EXPECT_TRUE(test.new_handler->remove_actors(ids[1], {1, 1}));
}

TEST(TestControllerHandler, GetActorsListNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_actor = set_actor(test.new_handler, id, 1, 1);
  check_get_actors_one_actor(test.new_handler, id, new_actor);
}

TEST(TestControllerHandler, GetActorsListMoreActors) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_actor1 = set_actor(test.new_handler, id, 1, 1);
  auto new_actor2 = set_actor(test.new_handler, id, 2, 2);
  auto new_actor3 = set_actor(test.new_handler, id, 3, 3);
  check_get_actors_three_actors(test.new_handler, id, new_actor1, new_actor2,
                                new_actor3);
}

TEST(TestControllerHandler, GetActorsListOverwriteActor) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_actor = set_actor(test.new_handler, id, 1, 1);
  check_get_actors_one_actor(test.new_handler, id, new_actor);
  new_actor = set_actor(test.new_handler, id, 2, 1);
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = test.new_handler->get_actors_list(id));
  EXPECT_EQ(actors[1], new_actor);
}

TEST(TestControllerHandler, GetActorsListMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  add_more_rooms(test.new_handler, ids);
  wolf::actor::actor new_actor;
  for (auto &id : ids) {
    new_actor = set_actor(test.new_handler, id, 1, 1);
    check_get_actors_one_actor(test.new_handler, id, new_actor);
  }
}

TEST(TestControllerHandler, GetActorsListOverwriteActorMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_3 ids;
  add_more_rooms(test.new_handler, ids);
  wolf::actor::actor new_actor;
  for (auto &id : ids) {
    new_actor = set_actor(test.new_handler, id, 1, 1);
    check_get_actors_one_actor(test.new_handler, id, new_actor);
  }
  new_actor = set_actor(test.new_handler, ids[1], 2, 1);
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = test.new_handler->get_actors_list(ids[1]));
  EXPECT_EQ(actors[1], new_actor);
}

TEST(TestControllerHandler, AddGetRemoveActors) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_actor = set_actor(test.new_handler, id, 1, 1);
  check_get_actors_one_actor(test.new_handler, id, new_actor);
  EXPECT_NO_THROW(test.new_handler->remove_actors(id, {1, 1}));
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = test.new_handler->get_actors_list(id));
  for (unsigned int index = 0; index < 16; ++index)
    EXPECT_EQ(actors[index], boost::none);
}

TEST(TestControllerHandler, AddGetRemoveActorsMoreActors) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  auto id = add_room(test.new_handler, 1);
  auto new_actor1 = set_actor(test.new_handler, id, 1, 1);
  auto new_actor2 = set_actor(test.new_handler, id, 2, 2);
  auto new_actor3 = set_actor(test.new_handler, id, 3, 3);
  check_get_actors_three_actors(test.new_handler, id, new_actor1, new_actor2,
                                new_actor3);
  EXPECT_TRUE(test.new_handler->remove_actors(id, {1, 3}));
  wolf::controller::controller::actor_array actors;
  EXPECT_NO_THROW(actors = test.new_handler->get_actors_list(id));
  for (unsigned int index = 0; index < 16; ++index)
    EXPECT_EQ(actors[index], boost::none);
}

TEST(TestControllerHandler, HandleControlLoopsNoError) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  set_active_states(test.new_handler, id, 0);
  add_all_actors(test.new_handler, id, 0);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_no_actors_set(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS1) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{true, false, false, false}});
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_humidity(65.f);
  values_handler.set_temperature(30.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas1(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS2Ventilation) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{false, true, false, false}});
  set_limits(test.new_handler, id, 0.f);
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_humidity(65.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas2_ventilation(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS2Dehumidifier) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{false, true, false, false}});
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_humidity(85.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas2_dehumidifier(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS3Ventilation) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{false, false, true, false}});
  set_limits(test.new_handler, id, 0.f);
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_humidity(70.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas3_ventilation(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS3Heating) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{false, false, true, false}});
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_temperature(16.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas3_heating(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsCheckActorsSVAS4) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  auto id = add_room(test.new_handler, 1);
  auto room = make_room(id);
  test.new_handler->set_active_states(id, {{false, false, false, true}});
  add_all_actors(test.new_handler, id, 0);
  values_handler.set_co2(850.f);
  values_handler.set_sprout_value(70.f);
  values_handler.set_voc(200.f);
  EXPECT_TRUE(test.new_handler->handle_control_loops(room, false));
  check_actors_svas4(test.new_handler, id);
}

TEST(TestControllerHandler, HandleControlLoopsMoreRooms) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  id_array_6 ids;
  auto rooms = make_rooms(test.new_handler, ids);
  set_active_states_more_rooms(test.new_handler, ids);
  add_all_actors_more_rooms(test.new_handler, ids);
  set_limits(test.new_handler, ids[1], 0.f);
  set_values(values_handler);
  control_all_rooms(test.new_handler, rooms, values_handler);
  check_all_rooms(test.new_handler, ids);
}

TEST(TestControllerHandler, HandleControlLoopsNoOutdoorSensor) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  id_array_6 ids;
  auto rooms = make_rooms(test.new_handler, ids);
  set_active_states_more_rooms(test.new_handler, ids);
  add_all_actors_more_rooms(test.new_handler, ids);
  set_limits(test.new_handler, ids[1], 0.f);
  set_values(values_handler);
  control_all_rooms(test.new_handler, rooms, values_handler);
  check_all_rooms_no_outdoor(test.new_handler, ids);
}

TEST(TestControllerHandler, HandleControlLoopsOutdoorTemperatureTooLow) {
  boost::asio::io_service io_service;
  fixture_value_handler_mock values_handler(io_service);
  controller_handler_test test(values_handler);
  set_outdoor(test.new_handler);
  id_array_6 ids;
  values_handler.set_temperature(-20.f);
  auto rooms = make_rooms(test.new_handler, ids);
  set_active_states_more_rooms(test.new_handler, ids);
  add_all_actors_more_rooms(test.new_handler, ids);
  set_limits(test.new_handler, ids[1], 0.f);
  set_values(values_handler);
  control_all_rooms(test.new_handler, rooms, values_handler);
  check_all_rooms_no_outdoor(test.new_handler, ids);
}
}  // namespace controller_handler_test
