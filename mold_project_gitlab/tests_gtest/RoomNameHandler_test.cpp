#include "mold/gui/RoomNameHandler.hpp"
#include "gtest/gtest.h"
#include "wolf/uuid_generator.hpp"

class dataExchangeMock : public mold::DataExchange {
 public:
  dataExchangeMock() : mold::DataExchange(mold::WolfServices{}) {}
  const rooms& get_rooms() const override {
    if (m_count == 0) {
      m_rooms.clear();
      return m_rooms;
    };
    m_rooms.resize(static_cast<unsigned long>(m_count));
    std::string name = "Room ";
    int name_count = 1;
    for (auto& room : m_rooms) {
      room.id = wolf::uuid_generator::generate_as_array();
      room.name = name + std::to_string(name_count);
      ++name_count;
    }
    return m_rooms;
  }

  void change_count(const int count) { m_count = count; }

 private:
  int m_count = 0;
  mutable rooms m_rooms;
};

static QStringList make_check_list() {
  return {"Living room", "Nursery", "Bedroom", "Kitchen",
          "Cellar",      "Office",  "Bathroom"};
}

static void add_name(const mold::RoomNameHandler& handler,
                     QStringList& room_list, const QString& room_name) {
  handler.addIfNewName(room_list, room_name);
}

static void check_name_index(const mold::RoomNameHandler& handler,
                             const QStringList& room_list,
                             const QString& room_name, const int check_index) {
  auto index = handler.getNameIndex(room_list, room_name);
  EXPECT_EQ(index, check_index);
}

TEST(RoomNameHandler, GenerateRoomList) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto list_handler = handler.generateRoomList();
  auto check_list = make_check_list();
  EXPECT_EQ(check_list, list_handler);
}

TEST(RoomNameHandler, AddIfNewNameNew) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto room_list = handler.generateRoomList();
  EXPECT_EQ(room_list.size(), 7);
  const QString new_name = "New Name";
  add_name(handler, room_list, new_name);
  EXPECT_EQ(room_list.size(), 8);
  EXPECT_EQ(room_list[0], new_name);
}

TEST(RoomNameHandler, AddIfNewNameNotNew) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto room_list = handler.generateRoomList();
  EXPECT_EQ(room_list.size(), 7);
  add_name(handler, room_list, "Bedroom");
  EXPECT_EQ(room_list.size(), 7);
}

TEST(RoomNameHandler, GetNameIndex) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto room_list = handler.generateRoomList();
  check_name_index(handler, room_list, "Living room", 0);
  check_name_index(handler, room_list, "Nursery", 1);
  check_name_index(handler, room_list, "Bedroom", 2);
  check_name_index(handler, room_list, "Kitchen", 3);
  check_name_index(handler, room_list, "Cellar", 4);
  check_name_index(handler, room_list, "Office", 5);
  check_name_index(handler, room_list, "Bathroom", 6);
}

TEST(RoomNameHandler, GetNameIndexNewRoom) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto room_list = handler.generateRoomList();
  check_name_index(handler, room_list, "Living room", 0);
  check_name_index(handler, room_list, "Nursery", 1);
  check_name_index(handler, room_list, "Bedroom", 2);
  check_name_index(handler, room_list, "Kitchen", 3);
  check_name_index(handler, room_list, "Cellar", 4);
  check_name_index(handler, room_list, "Office", 5);
  check_name_index(handler, room_list, "Bathroom", 6);
  add_name(handler, room_list, "New Room");
  check_name_index(handler, room_list, "New Room", 0);
  check_name_index(handler, room_list, "Living room", 1);
  check_name_index(handler, room_list, "Nursery", 2);
  check_name_index(handler, room_list, "Bedroom", 3);
  check_name_index(handler, room_list, "Kitchen", 4);
  check_name_index(handler, room_list, "Cellar", 5);
  check_name_index(handler, room_list, "Office", 6);
  check_name_index(handler, room_list, "Bathroom", 7);
}

TEST(RoomNameHandler, GetDefaultRoomName) {
  dataExchangeMock dataExchange;
  mold::RoomNameHandler handler(dataExchange);
  auto default_name = handler.getDefaultRoomName();
  EXPECT_EQ(default_name, "Room 1");
  dataExchange.change_count(4);
  default_name = handler.getDefaultRoomName();
  EXPECT_EQ(default_name, "Room 5");
  dataExchange.change_count(30);
  default_name = handler.getDefaultRoomName();
  EXPECT_EQ(default_name, "Room 31");
}
