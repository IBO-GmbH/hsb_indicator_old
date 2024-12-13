#include "wolf/controller/controller_cache.hpp"
#include "gtest/gtest.h"
#include "wolf/boost_serialize_helper.hpp"
#include "wolf/controller/boost_serialize_values_per_room.hpp"
#include "wolf/database.hpp"

#include <boost/serialization/vector.hpp>

using value_array = std::array<wolf::controller::values_per_room, 3>;

struct controller_cache_test {
  logging::logger test_logger;
  wolf::database new_database{":memory:"};
  std::unique_ptr<wolf::controller::controller_cache> new_cache;

  controller_cache_test() : test_logger("controller_cache_test") {
    new_cache.reset(new wolf::controller::controller_cache(new_database));
  }

  ~controller_cache_test() { new_database.close(); }
};

wolf::types::uuid_array make_room_id(const unsigned int digit) {
  wolf::types::uuid_array id_array;
  std::fill(id_array.begin(), id_array.end(), digit);
  return id_array;
}

void add_actors(wolf::controller::controller::actor_array& actors) {
  unsigned int actor_id = 0;
  for (auto& actor : actors) {
    ++actor_id;
    if (actor_id % 2 == 1) continue;
    actor = wolf::actor::actor{actor_id, true};
  }
}

wolf::controller::limits make_limits(const float change) {
  wolf::controller::limits new_limits;
  new_limits.humidity.max = 0.9f + change;
  new_limits.humidity.drH = 0.3f + change;
  new_limits.humidity_border = 0.7f + change;
  new_limits.temperature_.min = 17.f + change;
  new_limits.temperature_.reference = 22.f + change;
  new_limits.air_quality_.co2 = 25.f + change;
  new_limits.air_quality_.sprout_value = 0.5f + change;
  new_limits.air_quality_.voc = 80.f + change;
  return new_limits;
}

void set_loops_active(wolf::controller::values_per_room::active_states& states,
                      const bool set) {
  for (auto& state : states) state = set;
}

wolf::controller::values_per_room make_values(const unsigned int digit,
                                              const float change) {
  wolf::controller::values_per_room values;
  values.room_id = make_room_id(digit);
  add_actors(values.actors);
  values.limits_ = make_limits(change);
  set_loops_active(values.active_loops, true);
  return values;
}

wolf::controller::values_per_room add_value(controller_cache_test& new_test,
                                            const unsigned int digit,
                                            const float change) {
  auto value = make_values(digit, change);
  EXPECT_TRUE(new_test.new_cache->add(value));
  return value;
}

wolf::controller::values_per_room update_value(controller_cache_test& new_test,
                                               const unsigned int digit,
                                               const float change) {
  auto value = make_values(digit, change);
  EXPECT_TRUE(new_test.new_cache->update(value));
  return value;
}

void remove_value(controller_cache_test& new_test, const unsigned int digit) {
  auto id = make_room_id(digit);
  EXPECT_TRUE(new_test.new_cache->remove(id));
}

wolf::controller::controller_cache::values_list get_all(
    controller_cache_test& new_test, const std::size_t& size) {
  wolf::controller::controller_cache::values_list list;
  EXPECT_NO_THROW(list = new_test.new_cache->get_all());
  EXPECT_EQ(list.size(), size);
  return list;
}

void add_more_values(controller_cache_test& new_test, value_array& values) {
  unsigned int count = 1;
  for (auto& value : values) {
    value = add_value(new_test, count, static_cast<float>(count) / 10);
    ++count;
  }
}

void update_more_values(controller_cache_test& new_test, value_array& values) {
  unsigned int count = 1;
  for (auto& value : values) {
    value = update_value(new_test, count, static_cast<float>(count) / 5);
    ++count;
  }
}

void check_more_values(wolf::controller::controller_cache::values_list& list,
                       value_array& values) {
  unsigned int count = 0;
  for (auto& value_list : list) {
    EXPECT_EQ(value_list, values[count]);
    ++count;
  }
}

class database_test : public wolf::controller::controller_cache {
 public:
  database_test(wolf::database& database_)
      : wolf::controller::controller_cache(database_) {}

  void check() {
    values_list list1;
    list1.push_back(make_values(1, 0.1f));
    list1.push_back(make_values(2, 0.2f));
    list1.push_back(make_values(3, 0.3f));
    const std::string value = wolf::serialize::encode_to_base64(list1);
    write("values", value);
    const auto saved = get("values");
    values_list list2;
    wolf::serialize::decode_from_base64(saved, list2);
    EXPECT_EQ(list1, list2);
  }
};

TEST(TestControllerCache, AddControllerNoError) {
  controller_cache_test new_test;
  add_value(new_test, 1, 0.1f);
}

TEST(TestControllerCache, AddControllerAlreadyAdded) {
  controller_cache_test new_test;
  auto value = add_value(new_test, 1, 0.1f);
  EXPECT_FALSE(new_test.new_cache->add(value));
}

TEST(TestControllerCache, AddControllerMoreRooms) {
  controller_cache_test new_test;
  for (unsigned int count = 1; count < 4; ++count)
    add_value(new_test, count, static_cast<float>(count) / 10);
}

TEST(TestControllerCache, UpdateControllerNoError) {
  controller_cache_test new_test;
  add_value(new_test, 1, 0.f);
  update_value(new_test, 1, 0.5f);
}

TEST(TestControllerCache, UpdateControllerNotAdded) {
  controller_cache_test new_test;
  auto value = make_values(1, 0.f);
  EXPECT_FALSE(new_test.new_cache->update(value));
}

TEST(TestControllerCache, UpdateControllerMoreRooms) {
  controller_cache_test new_test;
  for (unsigned int count = 1; count < 4; ++count)
    add_value(new_test, count, static_cast<float>(count) / 10);
  for (unsigned int count = 1; count < 4; ++count)
    update_value(new_test, count, static_cast<float>(count) / 5);
}

TEST(TestControllerCache, RemoveControllerNoError) {
  controller_cache_test new_test;
  add_value(new_test, 1, 0.f);
  remove_value(new_test, 1);
}

TEST(TestControllerCache, RemoveControllerNoAdded) {
  controller_cache_test new_test;
  auto id = make_room_id(1);
  EXPECT_FALSE(new_test.new_cache->remove(id));
}

TEST(TestControllerCache, RemoveControllerMoreRooms) {
  controller_cache_test new_test;
  for (unsigned int count = 1; count < 4; ++count)
    add_value(new_test, count, static_cast<float>(count) / 10);
  remove_value(new_test, 2);
}

TEST(TestControllerCache, GetAllNoError) {
  controller_cache_test new_test;
  get_all(new_test, 0);
}

TEST(TestControllerCache, GetAllOneRoomAdded) {
  controller_cache_test new_test;
  auto value = add_value(new_test, 1, 0.f);
  auto list = get_all(new_test, 1);
  EXPECT_EQ(list[0], value);
}

TEST(TestControllerCache, GetAllMoreRoomsAdded) {
  controller_cache_test new_test;
  value_array values;
  add_more_values(new_test, values);
  auto list = get_all(new_test, 3);
  check_more_values(list, values);
}

TEST(TestControllerCache, GetAllMoreRoomsAddedUpdate) {
  controller_cache_test new_test;
  value_array values;
  add_more_values(new_test, values);
  auto list = get_all(new_test, 3);
  check_more_values(list, values);
  update_more_values(new_test, values);
  list = get_all(new_test, 3);
  check_more_values(list, values);
}

TEST(TestControllerCache, GetAllMoreRoomsAddedRemove) {
  controller_cache_test new_test;
  value_array values;
  add_more_values(new_test, values);
  auto list = get_all(new_test, 3);
  check_more_values(list, values);
  remove_value(new_test, 2);
  list = get_all(new_test, 2);
  EXPECT_EQ(list[0], values[0]);
  EXPECT_EQ(list[1], values[2]);
}

TEST(TestControllerCache, TestDatabase) {
  wolf::database new_database{":memory:"};
  std::unique_ptr<database_test> new_test;
  new_test.reset(new database_test(new_database));
  new_test->check();
  new_database.close();
}
