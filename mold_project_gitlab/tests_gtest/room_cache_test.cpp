#include "wolf/room_cache.hpp"
#include "gtest/gtest.h"
#include "wolf/database.hpp"
#include "wolf/room.hpp"

struct room_cache_test {
  wolf::database new_database;
  std::unique_ptr<wolf::room_cache> new_cache;
  room_cache_test() {
    new_database.open(":memory:");
    new_cache.reset(new wolf::room_cache(new_database));
  }
  void make_room(wolf::room& new_room, std::string name, unsigned char id,
                 wolf::sensor_value_type type) {
    wolf::types::uuid_array room_id;
    make_room_id(room_id, id);
    new_room.id = room_id;
    new_room.name = name;
    wolf::sensor_id first_sensor({'a'}, type);
    new_room.sensors.push_back(first_sensor);
  }
  void make_room_id(wolf::types::uuid_array& room_id, unsigned char id) {
    std::fill(room_id.begin(), room_id.end(), id);
  }
  ~room_cache_test() { new_database.close(); }
};

TEST(TestRoomCache, AddNewRoomNoError) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  EXPECT_NO_THROW(new_test.new_cache->add(new_room));
}

TEST(TestRoomCache, UpdateRoomNoError) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  std::int64_t sync;
  wolf::room updated_room;
  new_test.new_cache->add(new_room);
  new_test.make_room(updated_room, "updated room", 1,
                     wolf::sensor_value_type::humidity);
  EXPECT_NO_THROW(new_test.new_cache->update(updated_room));
}

TEST(TestRoomCache, RemoveRoomNoError) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  wolf::types::uuid_array room_id;
  new_test.make_room_id(room_id, 1);
  new_test.new_cache->add(new_room);
  EXPECT_NO_THROW(new_test.new_cache->remove(room_id));
}

TEST(TestRoomCache, GetAllAddedRooms) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  new_test.new_cache->add(new_room);
  wolf::room another_new_room;
  new_test.make_room(another_new_room, "another new room", 2,
                     wolf::sensor_value_type::humidity);
  new_test.new_cache->add(another_new_room);
  wolf::room_cache::rooms rooms;
  rooms = new_test.new_cache->get_all();
  EXPECT_EQ(rooms[0], new_room);
  EXPECT_EQ(rooms[1], another_new_room);
  EXPECT_NE(rooms[0], another_new_room);
  EXPECT_NE(rooms[1], new_room);
}

TEST(TestRoomCache, GetAddedAndUpdatedRooms) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  new_test.new_cache->add(new_room);
  wolf::room another_new_room;
  new_test.make_room(another_new_room, "another new room", 2,
                     wolf::sensor_value_type::humidity);
  new_test.new_cache->add(another_new_room);
  wolf::room_cache::rooms rooms;
  rooms = new_test.new_cache->get_all();
  EXPECT_EQ(rooms[0], new_room);
  EXPECT_EQ(rooms[1], another_new_room);
  EXPECT_NE(rooms[0], another_new_room);
  EXPECT_NE(rooms[1], new_room);
  wolf::room updated_room;
  new_test.make_room(updated_room, "updated room", 1,
                     wolf::sensor_value_type::humidity);
  new_test.new_cache->update(updated_room);
  rooms = new_test.new_cache->get_all();
  EXPECT_EQ(rooms[0], updated_room);
  EXPECT_EQ(rooms[1], another_new_room);
  EXPECT_NE(rooms[0], another_new_room);
  EXPECT_NE(rooms[1], updated_room);
  EXPECT_NE(rooms[0], new_room);
}

TEST(TestRoomCache, GetChangedRooms) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  new_test.new_cache->add(new_room);
  wolf::room another_new_room;
  new_test.make_room(another_new_room, "another new room", 2,
                     wolf::sensor_value_type::humidity);
  new_test.new_cache->add(another_new_room);
  wolf::room_cache::rooms rooms;
  rooms = new_test.new_cache->get_all();
  EXPECT_EQ(rooms[0], new_room);
  EXPECT_EQ(rooms[1], another_new_room);
  EXPECT_NE(rooms[0], another_new_room);
  EXPECT_NE(rooms[1], new_room);
  wolf::types::uuid_array room_id;
  new_test.make_room_id(room_id, 1);
  new_test.new_cache->remove(room_id);
  rooms = new_test.new_cache->get_all();
  EXPECT_EQ(rooms.size(), 1);
}

TEST(TestRoomCache, SetSentRoomNoError) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  EXPECT_NO_THROW(new_test.new_cache->add(new_room));
}

TEST(TestRoomCache, GetUnsyncedRoomNoError) {
  room_cache_test new_test;
  wolf::room new_room;
  new_test.make_room(new_room, "new room", 1,
                     wolf::sensor_value_type::temperature);
  EXPECT_NO_THROW(new_test.new_cache->add(new_room));
}
