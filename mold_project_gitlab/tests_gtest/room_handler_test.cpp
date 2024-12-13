#include "wolf/room_handler.hpp"
#include "gtest/gtest.h"

class room_cache_mock : public wolf::room_cache {
 public:
  room_cache_mock(wolf::database& database_) : wolf::room_cache{database_} {}
  void add(const wolf::room& to_add) override {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "Added: " << to_add.id << " " << to_add.name << " "
        << to_add.sensors[0];
  }
  void update(const wolf::room& to_update) override {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "Updated: " << to_update.id << " " << to_update.name << " "
        << to_update.sensors[0];
  }
  void remove(const wolf::types::uuid_array& to_remove) override {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "Removed: " << to_remove << " ";
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

  rooms get_all() override {
    wolf::room new_room;
    make_room(new_room, "new room", 1, wolf::sensor_value_type::temperature);
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "Room ID: " << new_room.id << " Room Name: " << new_room.name
        << " Sensors: " << new_room.sensors[0];
    rooms return_value;
    return_value.push_back(new_room);
    return return_value;
  }
};

void add_more_rooms(room_cache_mock& new_test, wolf::room_handler& new_handler,
                    wolf::room_cache::rooms& rooms) {
  wolf::room first_new_room;
  new_test.make_room(first_new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  new_handler.add(first_new_room);
  wolf::room second_new_room;
  new_test.make_room(second_new_room, "second new handle room", 3,
                     wolf::sensor_value_type::humidity);
  new_handler.add(second_new_room);
  wolf::room third_new_room;
  new_test.make_room(third_new_room, "second new handle room", 4,
                     wolf::sensor_value_type::co2);
  new_handler.add(third_new_room);
  rooms = new_handler.get_all();
}

TEST(TestRoomHandler, AddNewRoomNoError) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room new_room;
  new_test.make_room(new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  EXPECT_NO_THROW(new_handler.add(new_room));
}

TEST(TestRoomHandler, UpdateRoomNoError) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room new_room;
  new_test.make_room(new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  new_handler.add(new_room);
  wolf::room updated_room;
  new_test.make_room(updated_room, "updated handle room", 2,
                     wolf::sensor_value_type::humidity);
  EXPECT_NO_THROW(new_handler.update(updated_room));
}

TEST(TestRoomHandler, RemoveRoomNoError) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room new_room;
  new_test.make_room(new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  new_handler.add(new_room);
  wolf::types::uuid_array room_id;
  new_test.make_room_id(room_id, 1);
  EXPECT_NO_THROW(new_handler.remove(room_id));
}

TEST(TestRoomHandler, GetAllRooms) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room new_room;
  new_test.make_room(new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  wolf::room_cache::rooms rooms;
  EXPECT_NO_THROW(rooms = new_handler.get_all());
  EXPECT_EQ(rooms.size(), 1);
  new_handler.add(new_room);
  EXPECT_NO_THROW(rooms = new_handler.get_all());
  EXPECT_EQ(rooms.size(), 2);
}

TEST(TestRoomHandler, GetOneRoom) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room new_room;
  new_test.make_room(new_room, "new handle room", 2,
                     wolf::sensor_value_type::temperature);
  new_handler.add(new_room);
  wolf::room room;
  wolf::types::uuid_array room_id;
  new_test.make_room_id(room_id, 2);
  EXPECT_NO_THROW(room = new_handler.get(room_id));
  EXPECT_EQ(room, new_room);
}

TEST(TestRoomHandler, ReplaceSensorInAllRooms2) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room_cache::rooms rooms;
  add_more_rooms(new_test, new_handler, rooms);
  EXPECT_EQ(rooms.size(), 4);
  wolf::sensor_id test_id_1({'a'}, wolf::sensor_value_type::co2);
  wolf::sensor_id test_id_2({'a'}, wolf::sensor_value_type::temperature);
  wolf::sensor_id test_id_3({'a'}, wolf::sensor_value_type::humidity);
  wolf::sensor_id id_old({'a'}, wolf::sensor_value_type::co2);
  wolf::sensor_id id_new({'a'}, wolf::sensor_value_type::temperature);
  new_handler.replace_one_or_add_sensor_in_all(id_old, id_new);
  rooms = new_handler.get_all();
  EXPECT_EQ(rooms.size(), 4);
  EXPECT_EQ(rooms[0].sensors[0], test_id_2);
  EXPECT_EQ(rooms[1].sensors[0], test_id_2);
  EXPECT_EQ(rooms[2].sensors[0], test_id_3);
  EXPECT_EQ(rooms[3].sensors[0], test_id_2);
}

TEST(TestRoomHandler, AddSensorInAllRooms) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room_cache::rooms rooms;
  add_more_rooms(new_test, new_handler, rooms);
  EXPECT_EQ(rooms.size(), 4);
  const wolf::sensor_id id_old({'b'}, wolf::sensor_value_type::unknown);
  const wolf::sensor_id id_new({'b'}, wolf::sensor_value_type::temperature);
  new_handler.replace_one_or_add_sensor_in_all(id_old, id_new);
  rooms = new_handler.get_all();
  EXPECT_EQ(rooms.size(), 4);
  EXPECT_EQ(rooms[0].sensors[1], id_new);
  EXPECT_EQ(rooms[1].sensors[1], id_new);
  EXPECT_EQ(rooms[2].sensors[1], id_new);
  EXPECT_EQ(rooms[3].sensors[1], id_new);
}

TEST(TestRoomHandler, RemoveSensorInAllRooms) {
  wolf::database database{":memory:"};
  room_cache_mock new_test{database};
  wolf::room_handler new_handler(new_test, false);
  wolf::room_cache::rooms rooms;
  add_more_rooms(new_test, new_handler, rooms);
  EXPECT_EQ(rooms.size(), 4);
  wolf::sensor_id id_old_add({}, wolf::sensor_value_type::unknown);
  wolf::sensor_id id_new_add({'b'}, wolf::sensor_value_type::temperature);
  new_handler.replace_one_or_add_sensor_in_all(id_old_add, id_new_add);
  rooms = new_handler.get_all();
  EXPECT_EQ(rooms.size(), 4);
  wolf::sensor_id id_old_remove({'b'}, wolf::sensor_value_type::temperature);
  wolf::sensor_id id_new_remove({}, wolf::sensor_value_type::unknown);
  new_handler.replace_one_or_add_sensor_in_all(id_old_remove, id_new_remove);
  rooms = new_handler.get_all();
  EXPECT_EQ(rooms.size(), 4);
  EXPECT_EQ(rooms[0].sensors.size(), 1);
  EXPECT_EQ(rooms[1].sensors.size(), 1);
  EXPECT_EQ(rooms[2].sensors.size(), 1);
  EXPECT_EQ(rooms[3].sensors.size(), 1);
}

TEST(TestRoomHandler, ReplaceSensorTypeNoneWithNone) {
  wolf::database database{":memory:"};
  wolf::room_cache cache{database};
  wolf::room_handler new_handler(cache, false);
  auto rooms = new_handler.get_all();
  ASSERT_TRUE(rooms.empty());
  wolf::room room;
  {
    room_cache_mock new_test{database};
    new_test.make_room(room, "room", 2, wolf::sensor_value_type::temperature);
  }

  new_handler.add(room);

  rooms = new_handler.get_all();
  ASSERT_EQ(rooms.size(), 1);
  EXPECT_EQ(rooms[0].sensors.size(), 1);

  const wolf::sensor_id test_id_none({}, wolf::sensor_value_type::unknown);
  new_handler.replace_one_or_add_sensor_in_all(test_id_none, test_id_none);

  rooms = new_handler.get_all();
  ASSERT_EQ(rooms.size(), 1);
  EXPECT_EQ(rooms[0].sensors.size(), 1);
}
