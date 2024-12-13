#include "gtest/gtest.h"
#include "mold/network/sprout_type_helper.hpp"
#include "mold/sync/mold_sprout/sender.hpp"
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/timestamp_helper.hpp"
#include "wolf/uuid_helper.hpp"

namespace mold_sprout_sender_test {
using namespace logging;

static logger test_logger("mold_sprout_sender_test");

using mold_sprout_value = mold::sync::mold_sprout::sender::mold_sprout_value;
using optional_message = std::optional<Clima::Message>;

class websocket_sync_mock : public wolf::websocket_sync {
 public:
  websocket_sync_mock(boost::asio::io_service& service)
      : wolf::websocket_sync(service, {0, 0, false}) {}
  bool send(const package& to_send) override {
    LOGGING_LOG(test_logger, severity::normal)
        << "send package with size: " << to_send.size();
    check_package(to_send, value);
    return true;
  }

  optional_message parse_message(const websocket_sync_mock::package& to_parse) {
    if (to_parse.empty()) {
      LOGGING_LOG(test_logger, severity::error)
          << "parse_message, to_parse.empty()";
      return optional_message();
    }
    const bond::InputBuffer input(to_parse.data(), to_parse.size());
    using reader = bond::CompactBinaryReader<bond::InputBuffer>;
    const reader reader_(input);
    Clima::Message message;
    bond::Deserialize(reader_, message);
    return optional_message(message);
  }

  Clima::MoldSprout deserialize_message(const Clima::Message& to_deserialize) {
    wolf::network::bond_deserializer deserializer;
    const auto message =
        deserializer.parse_message<Clima::MoldSprout>(to_deserialize.Entry);
    return message;
  }

  Clima::MoldSprout convert_values(const mold_sprout_value& to_check) {
    Clima::MoldSprout result;
    result.RoomUUID = wolf::uuid_helper().array_to_vector(to_check.room_id);
    result.SproutType = mold::network::sprout_type_helper().cast_to_network(
        to_check.value.sprout_type_);
    result.TimestampStart = wolf::timestamp_helper().cast_to_unix_seconds(
        to_check.value.timestamp_start);
    result.TimestampChange = wolf::timestamp_helper().cast_to_unix_seconds(
        to_check.value.timestamp_change);
    return result;
  }

  void check_package(const websocket_sync_mock::package& to_parse,
                     const mold_sprout_value& to_check) {
    auto message = parse_message(to_parse);
    if (!message) {
      LOGGING_LOG(test_logger, severity::error) << "could not parse";
      return;
    }
    auto deserialized = deserialize_message(message.value());
    auto check_values = convert_values(to_check);
    EXPECT_EQ(deserialized.RoomUUID, check_values.RoomUUID);
    EXPECT_EQ(deserialized.SproutType, check_values.SproutType);
    EXPECT_EQ(deserialized.TimestampStart, check_values.TimestampStart);
    EXPECT_EQ(deserialized.TimestampChange, check_values.TimestampChange);
  }

  void set_mold_value(const mold_sprout_value& to_set) { value = to_set; }
  mold_sprout_value value;
};

class response_mock : public wolf::network::response::simple {
 public:
  response_mock(wolf::network::response::message& message)
      : wolf::network::response::simple(message) {}
  void register_work_uuid(const wolf::types::uuid_array& work_id,
                          const callback& to_call) override {
    LOGGING_LOG(test_logger, severity::normal) << "work uuid: " << work_id;
    to_call({});
  }
};

mold_sprout_value make_value(const mold::sprout_type& type) {
  auto id = wolf::uuid_generator::generate_as_array();
  auto now = std::chrono::system_clock::now();
  return {id, type, now, now};
}

auto callback = [](const std::optional<std::string>& string) {
  LOGGING_LOG(test_logger, severity::verbose)
      << "callback " << string.value_or("");
};

void handle_send(websocket_sync_mock& websocket,
                 const std::shared_ptr<mold::sync::mold_sprout::sender>& sender,
                 const mold::sprout_type& type) {
  mold_sprout_value value = make_value(type);
  websocket.set_mold_value(value);
  sender->send(value, callback);
}

TEST(TestMoldSproutSender, EmptyValue) {
  boost::asio::io_service service;
  websocket_sync_mock websocket(service);
  wolf::network::bond_sender bond_sender(websocket);
  wolf::network::bond_parser bond_parser(websocket);
  wolf::network::response::message message(bond_parser, service,
                                           std::chrono::seconds(0));
  response_mock simple(message);
  auto sender =
      std::make_shared<mold::sync::mold_sprout::sender_implementation>(
          bond_sender, simple);
  websocket.send({});
}

TEST(TestMoldSproutSender, SendValue) {
  boost::asio::io_service service;
  websocket_sync_mock websocket(service);
  wolf::network::bond_sender bond_sender(websocket);
  wolf::network::bond_parser bond_parser(websocket);
  wolf::network::response::message message(bond_parser, service,
                                           std::chrono::seconds(0));
  response_mock simple(message);
  auto sender =
      std::make_shared<mold::sync::mold_sprout::sender_implementation>(
          bond_sender, simple);
  handle_send(websocket, sender, mold::sprout_type::one_day);
}

TEST(TestMoldSproutSender, SendMoreValues) {
  boost::asio::io_service service;
  websocket_sync_mock websocket(service);
  wolf::network::bond_sender bond_sender(websocket);
  wolf::network::bond_parser bond_parser(websocket);
  wolf::network::response::message message(bond_parser, service,
                                           std::chrono::seconds(0));
  response_mock simple(message);
  auto sender =
      std::make_shared<mold::sync::mold_sprout::sender_implementation>(
          bond_sender, simple);
  handle_send(websocket, sender, mold::sprout_type::one_day);
  handle_send(websocket, sender, mold::sprout_type::two_days);
  handle_send(websocket, sender, mold::sprout_type::four_days);
  handle_send(websocket, sender, mold::sprout_type::eight_days);
  handle_send(websocket, sender, mold::sprout_type::sixteen_days);
  handle_send(websocket, sender, mold::sprout_type::infinite_days);
  handle_send(websocket, sender, mold::sprout_type::two_days);
  handle_send(websocket, sender, mold::sprout_type::eight_days);
  handle_send(websocket, sender, mold::sprout_type::one_day);
}
}  // namespace mold_sprout_sender_test
