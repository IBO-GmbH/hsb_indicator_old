#include "mold/nightmode_handler.hpp"
#include "gtest/gtest.h"

namespace nightmode_handler_test {

static logging::logger test_logger{"nightmode_handler_test"};

using time_point = std::chrono::system_clock::time_point;
using seconds = std::chrono::seconds;
using milliseconds = std::chrono::milliseconds;

class display_handler_mock : public mold::display_handler_base {
 public:
  display_handler_mock() : mold::display_handler_base() {}
  void set_on(const bool on) override {
    LOGGING_LOG(test_logger, logging::severity::normal) << "set_on(): " << on;
  }
  bool display_is_locked() const override { return true; }
  void handle_states_update() override {
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "handle_states_update()";
  }
  void enable_states_update(const bool enable) override {
    if (!m_set) {
      LOGGING_LOG(test_logger, logging::severity::normal)
          << "enableStatusUpdate set to: " << enable;
      m_status_update = enable;
      m_set = true;
    }
  }

  bool getStatusUpdate() { return m_status_update; }

 private:
  bool m_set = false;
  bool m_status_update = false;
};

mold::config make_config() {
  mold::program_options_parser parser;
  bool end_programm = false;
  std::vector<char> program_path_mock{'t', 'e', 's', 't'};
  std::vector<char *> argv;
  argv.push_back(program_path_mock.data());
  parser.parse(argv.size(), argv.data(), "test", end_programm);
  return parser.get_config_mold();
}

void change_times(mold::config &config, mold::config::nightmode times) {
  config.nightmode_ = times;
}

void test_enable(mold::config &config, const std::int64_t &time_stamp,
                 const bool check) {
  auto display_handler = std::make_shared<display_handler_mock>();
  boost::asio::io_service service;
  auto nightmode = std::make_shared<mold::nightmode_handler>(
      config, nullptr, display_handler.get(), service);
  time_point time_to_check{std::chrono::seconds(time_stamp)};
  nightmode->enable();
  EXPECT_EQ(nightmode->check_if_active(time_to_check), check);
  auto status_update = display_handler->getStatusUpdate();
  EXPECT_EQ(status_update, !check);
}

void check_calculation(std::shared_ptr<mold::nightmode_handler> nightmode,
                       mold::config::time off, const std::int64_t &time_stamp_1,
                       const std::int64_t &time_stamp_check) {
  time_point time_to_check{std::chrono::seconds(time_stamp_1)};
  auto check = nightmode->calculate_time(time_to_check, off.hours, off.minutes);
  EXPECT_EQ(check, time_stamp_check);
}

TEST(TestNightmodeHandler, EnabledTimeBetween6AMAnd10PM) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+18:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544808600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeAfter10PMBeforeMidnight) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+23:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544826600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeBefore6AMAfterMidnight) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+04:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544758200;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTime6AM) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTime10PM) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821200;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTime6AM01) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763660;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTime5AM59) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+05:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763540;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTime10PM01) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821260;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTime9PM59) {
  auto config = make_config();
  // https://www.wolframalpha.com/input/?i=2018-12-14+21:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821140;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledNewConfigtimeTimeBetween6AMAnd10PM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+18:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544808600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledNewConfigtimeTimeAfter10PMBeforeMidnight) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+23:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544826600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeBefore6AMAfterMidnight) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+04:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544758200;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime6AM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime10PM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime6AM01) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763660;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime5AM59) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+05:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763540;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime10PM01) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821260;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime9PM59) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+21:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821140;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime6AM30) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544765400;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime6AM15) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:15:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544764500;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime6AM45) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:45:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544766300;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime10PM30) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544823000;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime10PM15) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:15:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544822100;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtime10PM45) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:45:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544823900;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler,
     EnabledNewConfigtimeDifferentMinutesTimeBetween6AMAnd10PM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+18:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544808600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler,
     EnabledNewConfigtimeDifferentMinutesTimeAfter10PMBeforeMidnight) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+23:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544826600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler,
     EnabledTimeNewConfigtimeDifferentMinutesBefore6AMAfterMidnight) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+04:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544758200;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes6AM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes10PM) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes6AM01) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763660;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes5AM59) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+05:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763540;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes10PM01) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:01:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821260;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes9PM59) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+21:59:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821140;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes6AM30) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544765400;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes6AM15) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:15:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544764500;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes6AM45) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:45:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544766300;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes10PM30) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544823000;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes10PM15) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:15:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544822100;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeDifferentMinutes10PM45) {
  auto config = make_config();
  change_times(config, {{22, 30}, {6, 15}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:45:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544823900;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameDay6AM) {
  auto config = make_config();
  change_times(config, {{1, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763600;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameDay1AM) {
  auto config = make_config();
  change_times(config, {{1, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+01:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544745600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameDay7AM) {
  auto config = make_config();
  change_times(config, {{1, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+07:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544767200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameDay10PM) {
  auto config = make_config();
  change_times(config, {{1, 30}, {6, 30}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameHour6AM30) {
  auto config = make_config();
  change_times(config, {{6, 15}, {6, 45}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:30:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544765400;
  test_enable(config, time_stamp, true);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameHour6AM) {
  auto config = make_config();
  change_times(config, {{6, 15}, {6, 45}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544763600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameHour1AM) {
  auto config = make_config();
  change_times(config, {{6, 15}, {6, 45}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+01:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544745600;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameHour7AM) {
  auto config = make_config();
  change_times(config, {{6, 15}, {6, 45}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+07:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544767200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, EnabledTimeNewConfigtimeSameHour10PM) {
  auto config = make_config();
  change_times(config, {{6, 15}, {6, 45}, 10});
  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  const std::int64_t time_stamp = 1544821200;
  test_enable(config, time_stamp, false);
}

TEST(TestNightmodeHandler, Disabled) {
  auto config = make_config();
  auto display_handler = std::make_shared<display_handler_mock>();
  boost::asio::io_service service;
  auto nightmode = std::make_shared<mold::nightmode_handler>(
      config, nullptr, display_handler.get(), service);
  nightmode->disable();
  auto status_update = display_handler->getStatusUpdate();
  EXPECT_EQ(status_update, true);
}

TEST(TestNightmodeHandler, CalculateInactiveTime) {
  auto config = make_config();
  auto display_handler = std::make_shared<display_handler_mock>();
  boost::asio::io_service service;
  auto nightmode = std::make_shared<mold::nightmode_handler>(
      config, nullptr, display_handler.get(), service);
  mold::config::time check_times{config.nightmode_.off.hours,
                                 config.nightmode_.off.minutes};
  // https://www.wolframalpha.com/input/?i=2018-12-15+06:00:00+CET+to+unix+timestamp
  std::int64_t time_stamp_check = 1544850000;

  // https://www.wolframalpha.com/input/?i=2018-12-14+23:30:00+CET+to+unix+timestamp
  std::int64_t time_stamp_1 = 1544826600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+04:30:00+CET+to+unix+timestamp
  time_stamp_1 = 1544844600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-14+18:30:00+CET+to+unix+timestamp
  time_stamp_1 = 1544808600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-14+06:01:00+CET+to+unix+timestamp
  time_stamp_1 = 1544763660;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-14+06:00:00+CET+to+unix+timestamp
  time_stamp_1 = 1544763600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+05:59:00+CET+to+unix+timestamp
  time_stamp_1 = 1544849940;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+00:00:00+CET+to+unix+timestamp
  time_stamp_1 = 1544828400;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);
}

TEST(TestNightmodeHandler, CalculateActiveTime) {
  auto config = make_config();
  auto display_handler = std::make_shared<display_handler_mock>();
  boost::asio::io_service service;
  auto nightmode = std::make_shared<mold::nightmode_handler>(
      config, nullptr, display_handler.get(), service);
  mold::config::time check_times{config.nightmode_.on.hours,
                                 config.nightmode_.on.minutes};
  // https://www.wolframalpha.com/input/?i=2018-12-15+22:00:00+CET+to+unix+timestamp
  std::int64_t time_stamp_check = 1544907600;

  // https://www.wolframalpha.com/input/?i=2018-12-14+23:30:00+CET+to+unix+timestamp
  std::int64_t time_stamp_1 = 1544826600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+04:30:00+CET+to+unix+timestamp
  time_stamp_1 = 1544844600;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+18:30:00+CET+to+unix+timestamp
  time_stamp_1 = 1544895000;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-14+22:01:00+CET+to+unix+timestamp
  time_stamp_1 = 1544821260;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-14+22:00:00+CET+to+unix+timestamp
  time_stamp_1 = 1544821200;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+21:59:00+CET+to+unix+timestamp
  time_stamp_1 = 1544907540;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);

  // https://www.wolframalpha.com/input/?i=2018-12-15+00:00:00+CET+to+unix+timestamp
  time_stamp_1 = 1544828400;
  check_calculation(nightmode, check_times, time_stamp_1, time_stamp_check);
}
}  // namespace nightmode_handler_test
