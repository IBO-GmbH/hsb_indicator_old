#include <boost/di/extension/scopes/shared.hpp>
#include "create_room.hpp"
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "mold/sync/mold_sprout/handler.hpp"

namespace di = boost::di;

struct mold_sprout_sender_mock : mold::sync::mold_sprout::sender {
  boost::asio::io_context &io_context;
  int send_call_count{};
  bool answer_success{true};

  mold_sprout_sender_mock(boost::asio::io_context &io_context)
      : io_context(io_context) {}
  void send(const mold_sprout_value & /*to_send*/,
            const callback_type &callback) override {
    ++send_call_count;
    io_context.post([this, callback] {
      if (answer_success) {
        callback({});
        return;
      }
      callback("error");
    });
  }
};

struct login_handler_mock : wolf::login_handler {
  bool logged_in{true};
  bool is_logged_in() const override { return logged_in; }
};

static auto create_injector(boost::asio::io_context &io_context) {
  return di::make_injector<di::extension::shared_config>(
      di::bind<boost::asio::io_service>().to(io_context),
      di::bind<wolf::database>().to<database_in_memory>(),
      di::bind<mold::sync::mold_sprout::sender>().to<mold_sprout_sender_mock>(),
      di::bind<wolf::login_handler>().to<login_handler_mock>());
}

struct SyncMoldSproutHandler : testing::Test {
  boost::asio::io_context io_context;
  decltype(create_injector(io_context)) injector = create_injector(io_context);
  mold::sprout::handler &sprout_handler =
      injector.create<mold::sprout::handler &>();
  decltype(create_room(injector)) room_config = create_room(injector);
  mold::sync::mold_sprout::cache &cache =
      injector.create<mold::sync::mold_sprout::cache &>();
  mold_sprout_sender_mock &sender =
      injector.create<mold_sprout_sender_mock &>();
  mold::sync::mold_sprout::handler &handler =
      injector.create<mold::sync::mold_sprout::handler &>();
};

static mold::sprout::data_types::value create_sprout_value(
    SyncMoldSproutHandler &test) {
  const auto now = std::chrono::system_clock::now();
  mold::sprout::data_types::value value;
  auto &infinity_timestamps_ = value.timestamps_[static_cast<std::size_t>(
      mold::sprout_type::infinite_days)];
  infinity_timestamps_.start = infinity_timestamps_.change = now;
  value.type_ = mold::sprout_type::infinite_days;
  value.configuration = test.room_config.configuration_.id;
  return value;
}

static void emit_one_change(SyncMoldSproutHandler &test) {
  const auto value = create_sprout_value(test);
  test.sprout_handler.signal_sprout_type_changed(value);
}

static void emit_two_changes(SyncMoldSproutHandler &test) {
  for (int counter{}; counter < 2; ++counter) emit_one_change(test);
}

TEST_F(SyncMoldSproutHandler, CheckSendCalled) {
  emit_one_change(*this);
  io_context.run();
  EXPECT_EQ(sender.send_call_count, 1);
}

TEST_F(SyncMoldSproutHandler, SendOneCheckAllSycned) {
  emit_one_change(*this);
  io_context.run();
  EXPECT_TRUE(cache.get_all().empty());
}

TEST_F(SyncMoldSproutHandler, SendTwo) {
  emit_two_changes(*this);
  io_context.run();
  EXPECT_EQ(sender.send_call_count, 2);
}

TEST_F(SyncMoldSproutHandler, SendTwoCheckAllSynced) {
  emit_two_changes(*this);
  io_context.run();
  EXPECT_TRUE(cache.get_all().empty());
}

TEST_F(SyncMoldSproutHandler, FailToSend) {
  sender.answer_success = false;
  emit_one_change(*this);
  io_context.run();
  EXPECT_EQ(sender.send_call_count, 1);
  EXPECT_EQ(cache.get_all().size(), 1);
}

TEST_F(SyncMoldSproutHandler, FailFirstAndResyncWithSuccess) {
  sender.answer_success = false;
  emit_one_change(*this);
  io_context.run();
  sender.answer_success = true;
  handler.resync();
  io_context.reset();
  io_context.run();
  EXPECT_EQ(sender.send_call_count, 2);
  EXPECT_TRUE(cache.get_all().empty());
}
