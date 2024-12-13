#include "mold/configuration_cache.hpp"
#include "gtest/gtest.h"
#include "mold/configuration.hpp"
#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace configuration_cache_test {

logging::logger logger("configuration_cache_test");

struct configuration_cache_test {
  wolf::database new_database;
  std::unique_ptr<mold::configuration_cache> new_cache;
  configuration_cache_test() {
    new_database.open(":memory:");
    new_cache.reset(new mold::configuration_cache(new_database));
  }

  void make_new_configuration(const unsigned char& config_digits,
                              const unsigned char& room_digits,
                              mold::configuration& configuration) {
    configuration.id = make_config_id(config_digits);
    configuration.room = make_config_id(room_digits);
  }

  wolf::types::uuid_array make_config_id(const unsigned char& config_digits) {
    wolf::types::uuid_array config_id;
    std::fill(config_id.begin(), config_id.end(), config_digits);
    return config_id;
  }

  wolf::types::uuid_array make_room_id(const unsigned char& room_digits) {
    wolf::types::uuid_array room_id;
    std::fill(room_id.begin(), room_id.end(), room_digits);
    return room_id;
  }

  ~configuration_cache_test() { new_database.close(); }
};

TEST(TestConfigurationCache, AddNewConfigurationNoError) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);
  EXPECT_NO_THROW(new_test.new_cache->add(new_configuration));
}

TEST(TestConfigurationCache, AddExistingConfiguration) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);
  new_test.new_cache->add(new_configuration);
  EXPECT_THROW(new_test.new_cache->add(new_configuration), std::runtime_error);
}

TEST(TestConfigurationCache, UpdateExistingConfigurationNoError) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);

  new_test.new_cache->add(new_configuration);
  new_test.make_new_configuration(1, 2, new_configuration);
  EXPECT_NO_THROW(new_test.new_cache->update(new_configuration));
}

TEST(TestConfigurationCache, UpdateNotExistingConfiguration) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);

  new_test.new_cache->add(new_configuration);
  new_test.make_new_configuration(2, 1, new_configuration);
  EXPECT_THROW(new_test.new_cache->update(new_configuration),
               std::runtime_error);
}

TEST(TestConfigurationCache, RemoveExistingConfigurationNoError) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);

  new_test.new_cache->add(new_configuration);
  new_test.make_new_configuration(1, 2, new_configuration);
  EXPECT_NO_THROW(new_test.new_cache->remove(new_configuration.id));
}

TEST(TestConfigurationCache, RemoveNotExistingConfiguration) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);

  new_test.new_cache->add(new_configuration);
  new_test.make_new_configuration(2, 1, new_configuration);
  EXPECT_THROW(new_test.new_cache->remove(new_configuration.id),
               std::runtime_error);
}

TEST(TestConfigurationCache, GetAllConfigurationsNoError) {
  configuration_cache_test new_test;
  mold::configuration new_configuration;
  new_test.make_new_configuration(1, 1, new_configuration);

  new_test.new_cache->add(new_configuration);
  new_test.make_new_configuration(2, 1, new_configuration);
  new_test.new_cache->add(new_configuration);
  mold::configuration_cache::configurations results;
  EXPECT_NO_THROW(results = new_test.new_cache->get_all());
  EXPECT_EQ(results.size(), 2);
  new_test.make_new_configuration(3, 1, new_configuration);
  new_test.new_cache->add(new_configuration);
  EXPECT_NO_THROW(results = new_test.new_cache->get_all());
  EXPECT_EQ(results.size(), 3);
}

TEST(TestConfigurationCache, AddUpdateAndGetConfigurations) {
  configuration_cache_test new_test;
  mold::configuration new_configuration_1;
  mold::configuration new_configuration_2;
  mold::configuration new_configuration_3;
  new_test.make_new_configuration(1, 1, new_configuration_1);

  new_test.new_cache->add(new_configuration_1);
  new_test.make_new_configuration(2, 1, new_configuration_2);
  new_test.new_cache->add(new_configuration_2);
  mold::configuration_cache::configurations results;
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(2));
  EXPECT_EQ(results[1].room, new_test.make_room_id(1));
  new_test.make_new_configuration(3, 1, new_configuration_3);
  new_test.new_cache->add(new_configuration_3);
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 3);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(2));
  EXPECT_EQ(results[1].room, new_test.make_room_id(1));
  EXPECT_EQ(results[2].id, new_test.make_config_id(3));
  EXPECT_EQ(results[2].room, new_test.make_room_id(1));
  for (auto& print : results)
    LOGGING_LOG(logger, logging::severity::normal) << print;
  new_test.make_new_configuration(2, 2, new_configuration_2);
  new_test.new_cache->update(new_configuration_2);
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 3);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(2));
  EXPECT_EQ(results[1].room, new_test.make_room_id(2));
  EXPECT_EQ(results[2].id, new_test.make_config_id(3));
  EXPECT_EQ(results[2].room, new_test.make_room_id(1));
  for (auto& print : results)
    LOGGING_LOG(logger, logging::severity::normal) << print;
}

TEST(TestConfigurationCache, AddRemoveAndGetConfigurations) {
  configuration_cache_test new_test;
  mold::configuration new_configuration_1;
  mold::configuration new_configuration_2;
  mold::configuration new_configuration_3;
  new_test.make_new_configuration(1, 1, new_configuration_1);

  new_test.new_cache->add(new_configuration_1);
  new_test.make_new_configuration(2, 1, new_configuration_2);
  new_test.new_cache->add(new_configuration_2);
  mold::configuration_cache::configurations results;
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(2));
  EXPECT_EQ(results[1].room, new_test.make_room_id(1));
  new_test.make_new_configuration(3, 1, new_configuration_3);
  new_test.new_cache->add(new_configuration_3);
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 3);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(2));
  EXPECT_EQ(results[1].room, new_test.make_room_id(1));
  EXPECT_EQ(results[2].id, new_test.make_config_id(3));
  EXPECT_EQ(results[2].room, new_test.make_room_id(1));
  for (auto& print : results)
    LOGGING_LOG(logger, logging::severity::normal) << print;
  new_test.new_cache->remove(new_configuration_2.id);
  results = new_test.new_cache->get_all();
  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].id, new_test.make_config_id(1));
  EXPECT_EQ(results[0].room, new_test.make_room_id(1));
  EXPECT_EQ(results[1].id, new_test.make_config_id(3));
  EXPECT_EQ(results[1].room, new_test.make_room_id(1));
  for (auto& print : results)
    LOGGING_LOG(logger, logging::severity::normal) << print;
}

}  // namespace configuration_cache_test
