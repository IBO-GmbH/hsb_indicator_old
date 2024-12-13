#include <boost/di/extension/scopes/shared.hpp>
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "mold/sync/room/settings/handler.hpp"

namespace di = boost::di;

static auto create_injector(boost::asio::io_service& service) {
  return di::make_injector<di::extension::shared_config>(
      di::bind<boost::asio::io_service>().to(service),
      di::bind<wolf::database>().to<database_in_memory>(),
      di::bind<wolf::login_handler>().to<wolf::login_handler_implementation>());
}

template <class create_type, class injector_type>
static std::shared_ptr<create_type> create(injector_type& injector) {
  return injector.template create<std::shared_ptr<create_type>>();
}

class SyncRoomSettings : public ::testing::Test {
 protected:
  boost::asio::io_service service;
  decltype(create_injector(service)) injector = create_injector(service);
  std::shared_ptr<wolf::room_handler> rooms =
      create<wolf::room_handler>(injector);
  std::shared_ptr<mold::sync::room::settings::parser> parser =
      create<mold::sync::room::settings::parser>(injector);
  std::shared_ptr<wolf::network::response::sender> sender =
      create<wolf::network::response::sender>(injector);
  std::shared_ptr<mold::sync::room::settings::handler> handler =
      create<mold::sync::room::settings::handler>(injector);
};

TEST_F(SyncRoomSettings, Update) {
  const wolf::types::uuid_array room_id{1};
  {
    const wolf::room to_add{room_id, "name", {}};
    rooms->add(to_add);
  }
  const std::string updated_name = "other name";
  {
    wolf::network::response::send_helper send_helper{*sender, {2}};
    mold::sync::room::settings::parser::information information{
        room_id, updated_name, mold::building_type::none, 0.f,
        mold::substrate_type::none};
    parser->signal_parsed(send_helper, information);
  }
  {
    ASSERT_EQ(rooms->get_all().size(), 1);
    const wolf::room check = rooms->get(room_id);
    EXPECT_EQ(check.name, updated_name);
    EXPECT_TRUE(check.sensors.empty());
  }
}
