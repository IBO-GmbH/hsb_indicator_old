#include "mold/comfort_questionnaire_cache.hpp"
#include "gtest/gtest.h"
#include "wolf/timestamp_helper.hpp"

namespace comfort_questionnaire_cache_test {

// chrono timestamp has an accuracy of nanoseconds, but the saved accuracy is
// only in seconds, so it needs to be converted to be treated as equal
static auto timestamp = wolf::timestamp_helper().cast_seconds_to_chrono(
    wolf::timestamp_helper().cast_to_unix_seconds(
        std::chrono::system_clock::now()));

struct comfort_questionnaire_cache_test {
  logging::logger test_logger;
  wolf::database new_database{":memory:"};
  std::unique_ptr<mold::comfort_questionnaire_cache> new_cache;

  comfort_questionnaire_cache_test()
      : test_logger("comfort_questionnaire_cache_test") {
    new_cache.reset(new mold::comfort_questionnaire_cache(new_database));
  }

  ~comfort_questionnaire_cache_test() { new_database.close(); }
};

mold::questionnaire_options make_options() {
  return {static_cast<bool>(std::rand() % 2),
          (1 + (std::rand() % 6)),
          {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21},
          {(std::rand() % 60), (std::rand() % 60), (std::rand() % 60),
           (std::rand() % 60), (std::rand() % 60), (std::rand() % 60),
           (std::rand() % 60), (std::rand() % 60), (std::rand() % 60),
           (std::rand() % 60), (std::rand() % 60), (std::rand() % 60),
           (std::rand() % 60), (std::rand() % 60), (std::rand() % 60)}};
}

TEST(TestComfortQuestionnaireCache, SetOptionsNoError) {
  comfort_questionnaire_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_options(make_options()));
}

TEST(TestComfortQuestionnaireCache, GetOptionsNotSet) {
  comfort_questionnaire_cache_test new_test;
  EXPECT_FALSE(new_test.new_cache->get_options().has_value());
}

TEST(TestComfortQuestionnaireCache, GetOptionsNoError) {
  comfort_questionnaire_cache_test new_test;
  const auto options = make_options();
  new_test.new_cache->set_options(options);
  const auto loaded_options = new_test.new_cache->get_options();
  EXPECT_TRUE(loaded_options.has_value());
  EXPECT_EQ(loaded_options.value(), options);
}

TEST(TestComfortQuestionnaireCache, ChangeOptions) {
  comfort_questionnaire_cache_test new_test;
  const auto options_1 = make_options();
  new_test.new_cache->set_options(options_1);
  auto loaded_options = new_test.new_cache->get_options();
  EXPECT_EQ(loaded_options.value(), options_1);
  const auto options_2 = make_options();
  new_test.new_cache->set_options(options_2);
  loaded_options = new_test.new_cache->get_options();
  EXPECT_NE(loaded_options.value(), options_1);
  EXPECT_EQ(loaded_options.value(), options_2);
}
}  // namespace comfort_questionnaire_cache_test
