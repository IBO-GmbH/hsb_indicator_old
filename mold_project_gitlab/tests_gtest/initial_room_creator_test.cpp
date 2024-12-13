#include "mold/initial_room_creator.hpp"
#include <boost/di/extension/scopes/shared.hpp>
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "mold/program_options_parser.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"

namespace initial_room_creator_test {

static logging::logger test_logger("initial_room_creator_test");

class room_handler_mock : public wolf::room_handler {
 public:
  room_handler_mock(wolf::room_cache& cache)
      : wolf::room_handler(cache, false) {}

  bool add(const wolf::room& to_add) override {
    wolf::room_handler::add(to_add);
    safe_room = to_add;
    return true;
  }

  wolf::room get_room() { return safe_room; }

 private:
  wolf::room safe_room;
};

static mold::config make_config() {
  mold::config config;
  config.initial_.room_name = "test room";
  config.initial_.room_id = "00000000-0000-0000-0000-000000000011";
  config.initial_.outdoor_sensor = 0xbeef;
  config.general_.eep = {0xa5, 0x04, 0x01};
  return config;
}

static void check_room_sensors(
    const std::shared_ptr<room_handler_mock>& room_handler,
    const int sensor_count) {
  auto room = room_handler->get_room();
  auto sensors = room.sensors;
  EXPECT_EQ(sensors.size(), sensor_count);
}

static auto create_injector(boost::asio::io_service& service) {
  namespace di = boost::di;
  auto config = make_config();
  return di::make_injector<di::extension::shared_config>(
      di::bind<boost::asio::io_service>().to(service),
      di::bind<wolf::database>().to<database_in_memory>(),
      di::bind<wolf::room_handler>().to<room_handler_mock>(),
      di::bind<mold::config>().to(std::move(config)));
}

namespace {
class TestInitialRoomCreator : public ::testing::Test {
 protected:
  boost::asio::io_service service;
  decltype(create_injector(service)) injector = create_injector(service);
  std::shared_ptr<room_handler_mock> room_handler =
      injector.create<std::shared_ptr<room_handler_mock>>();
  std::shared_ptr<mold::initial_room_creator> creator =
      injector.create<std::shared_ptr<mold::initial_room_creator>>();
};
}  // namespace

TEST_F(TestInitialRoomCreator, CreateEmptyRoom) {
  creator->create_no_room();
  creator->create_initial_room(false);
  auto rooms = room_handler->get_all();
  EXPECT_TRUE(rooms.empty());
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30) {
  creator->add_scd30({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSGPC3) {
  creator->add_sgpc3();
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 1);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25) {
  creator->add_sht25({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1) {
  creator->add_shtc1({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895) {
  creator->add_ee895({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 3);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85) {
  creator->add_sht85({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117) {
  creator->add_tmp117({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 1);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndSGPC3) {
  creator->add_sgpc3();
  creator->add_scd30({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndSHT25) {
  creator->add_scd30(wolf::sensor_type::sht25);
  creator->add_sht25(wolf::sensor_type::scd30);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndSHTC1) {
  creator->add_scd30(wolf::sensor_type::shtc1);
  creator->add_shtc1(wolf::sensor_type::scd30);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndEE895) {
  creator->add_scd30(wolf::sensor_type::ee895);
  creator->add_ee895(wolf::sensor_type::scd30);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndSHT85) {
  creator->add_scd30(wolf::sensor_type::sht85);
  creator->add_sht85(wolf::sensor_type::scd30);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSCD30AndTMP117) {
  creator->add_scd30(wolf::sensor_type::tmp117);
  creator->add_tmp117(wolf::sensor_type::scd30);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndSCD30) {
  creator->add_sht25(wolf::sensor_type::scd30);
  creator->add_scd30(wolf::sensor_type::sht25);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndSGPC3) {
  creator->add_sgpc3();
  creator->add_sht25({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndSHTC1) {
  creator->add_sht25({wolf::sensor_type::shtc1});
  creator->add_shtc1({wolf::sensor_type::sht25});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndEE895) {
  creator->add_sht25(wolf::sensor_type::ee895);
  creator->add_ee895(wolf::sensor_type::sht25);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndSHT85) {
  creator->add_sht25({wolf::sensor_type::sht85});
  creator->add_sht85({wolf::sensor_type::sht25});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT25AndTMP117) {
  creator->add_sht25({wolf::sensor_type::tmp117});
  creator->add_tmp117({wolf::sensor_type::sht25});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndSCD30) {
  creator->add_shtc1(wolf::sensor_type::scd30);
  creator->add_scd30(wolf::sensor_type::shtc1);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndSGPC3) {
  creator->add_sgpc3();
  creator->add_shtc1({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndSHT25) {
  creator->add_shtc1({wolf::sensor_type::sht25});
  creator->add_sht25({wolf::sensor_type::shtc1});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndEE895) {
  creator->add_shtc1(wolf::sensor_type::ee895);
  creator->add_ee895(wolf::sensor_type::shtc1);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndSHT85) {
  creator->add_shtc1({wolf::sensor_type::sht85});
  creator->add_sht85({wolf::sensor_type::shtc1});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHTC1AndTMP117) {
  creator->add_shtc1({wolf::sensor_type::tmp117});
  creator->add_tmp117({wolf::sensor_type::shtc1});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndSCD30) {
  creator->add_ee895(wolf::sensor_type::scd30);
  creator->add_scd30(wolf::sensor_type::ee895);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndSGPC3) {
  creator->add_sgpc3();
  creator->add_ee895({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndSHT25) {
  creator->add_ee895(wolf::sensor_type::sht25);
  creator->add_sht25(wolf::sensor_type::ee895);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndSHTC1) {
  creator->add_ee895(wolf::sensor_type::shtc1);
  creator->add_shtc1(wolf::sensor_type::ee895);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndSHT85) {
  creator->add_ee895(wolf::sensor_type::sht85);
  creator->add_sht85(wolf::sensor_type::ee895);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithEE895AndTMP117) {
  creator->add_ee895(wolf::sensor_type::tmp117);
  creator->add_tmp117(wolf::sensor_type::ee895);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 3);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndSCD30) {
  creator->add_sht85(wolf::sensor_type::scd30);
  creator->add_scd30(wolf::sensor_type::sht85);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndSGPC3) {
  creator->add_sgpc3();
  creator->add_sht85({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndSHT25) {
  creator->add_sht85({wolf::sensor_type::sht25});
  creator->add_sht25({wolf::sensor_type::sht85});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndSHTC1) {
  creator->add_sht85({wolf::sensor_type::shtc1});
  creator->add_shtc1({wolf::sensor_type::sht85});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndEE895) {
  creator->add_sht85(wolf::sensor_type::ee895);
  creator->add_ee895(wolf::sensor_type::sht85);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 6);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithSHT85AndTMP117) {
  creator->add_sht85({wolf::sensor_type::tmp117});
  creator->add_tmp117({wolf::sensor_type::sht85});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndSCD30) {
  creator->add_tmp117(wolf::sensor_type::scd30);
  creator->add_scd30(wolf::sensor_type::tmp117);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 5);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndSGPC3) {
  creator->add_sgpc3();
  creator->add_tmp117({});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 2);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndSHT25) {
  creator->add_tmp117({wolf::sensor_type::sht25});
  creator->add_sht25({wolf::sensor_type::tmp117});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndSHTC1) {
  creator->add_tmp117({wolf::sensor_type::shtc1});
  creator->add_shtc1({wolf::sensor_type::tmp117});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndEE895) {
  creator->add_tmp117(wolf::sensor_type::ee895);
  creator->add_ee895(wolf::sensor_type::tmp117);
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 3);
}

TEST_F(TestInitialRoomCreator, CreateRoomWithTMP117AndSHT85) {
  creator->add_tmp117({wolf::sensor_type::sht85});
  creator->add_sht85({wolf::sensor_type::tmp117});
  creator->create_initial_room(false);
  check_room_sensors(room_handler, 4);
}
}  // namespace initial_room_creator_test
