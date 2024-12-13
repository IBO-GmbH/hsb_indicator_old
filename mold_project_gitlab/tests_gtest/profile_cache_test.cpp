#include "wolf/profile_cache.hpp"
#include "gtest/gtest.h"
#include "wolf/database.hpp"
#include "wolf/sensor_profile.hpp"

struct profile_cache_test {
  wolf::database new_database;
  std::unique_ptr<wolf::profile_cache> new_cache;
  profile_cache_test() : t_logger("profile_cache_test") {
    new_database.open(":memory:");
    new_cache.reset(new wolf::profile_cache(new_database));
  }
  void make_sensor_profile(wolf::sensor_profile& new_sensor_profile,
                           const wolf::sensor_type sensor_type,
                           const wolf::sensor_value_type value_type,
                           const std::uint8_t& id) {
    new_sensor_profile.id = make_sensor_profile_id(value_type, id);
    new_sensor_profile.type = sensor_type;
  }
  wolf::sensor_id make_sensor_profile_id(
      const wolf::sensor_value_type value_type, const std::uint8_t& id) {
    wolf::types::id id_casted = {id};
    wolf::sensor_id sensor_id(id_casted, value_type);
    return sensor_id;
  }
  ~profile_cache_test() { new_database.close(); }

  logging::logger t_logger;
};

TEST(TestProfileCache, AddNewSensorProfileNoError) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  EXPECT_NO_THROW(new_test.new_cache->add(new_sensor_profile));
}

TEST(TestProfileCache, UpdateSensorProfileNoError) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  wolf::sensor_profile updated_sensor_profile;
  new_test.new_cache->add(new_sensor_profile);
  new_test.make_sensor_profile(updated_sensor_profile, wolf::sensor_type::scd30,
                               wolf::sensor_value_type::temperature, 1);
  EXPECT_NO_THROW(new_test.new_cache->update(updated_sensor_profile));
}

TEST(TestProfileCache, RemoveSensorProfileNoError) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  wolf::sensor_id sensor_profile_id;
  sensor_profile_id =
      new_test.make_sensor_profile_id(wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->add(new_sensor_profile);
  EXPECT_NO_THROW(new_test.new_cache->remove(sensor_profile_id));
}

TEST(TestProfileCache, GetAllAddedSensorProfiles) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->add(new_sensor_profile);
  wolf::sensor_profile another_new_sensor_profile;
  new_test.make_sensor_profile(another_new_sensor_profile,
                               wolf::sensor_type::enocean,
                               wolf::sensor_value_type::temperature, 2);
  new_test.new_cache->add(another_new_sensor_profile);
  wolf::profile_cache::get_all_result sensor_profiles;
  sensor_profiles = new_test.new_cache->get_all();
  EXPECT_EQ(sensor_profiles[0], new_sensor_profile);
  EXPECT_EQ(sensor_profiles[1], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[0], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[1], new_sensor_profile);
}

TEST(TestProfileCache, GetAddedAndUpdatedSensorProfiles) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->add(new_sensor_profile);
  wolf::sensor_profile another_new_sensor_profile;
  new_test.make_sensor_profile(another_new_sensor_profile,
                               wolf::sensor_type::enocean,
                               wolf::sensor_value_type::temperature, 2);
  new_test.new_cache->add(another_new_sensor_profile);
  wolf::profile_cache::get_all_result sensor_profiles;
  sensor_profiles = new_test.new_cache->get_all();
  EXPECT_EQ(sensor_profiles[0], new_sensor_profile);
  EXPECT_EQ(sensor_profiles[1], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[0], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[1], new_sensor_profile);
  wolf::sensor_profile updated_sensor_profile;
  new_test.make_sensor_profile(updated_sensor_profile, wolf::sensor_type::scd30,
                               wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->update(updated_sensor_profile);
  sensor_profiles = new_test.new_cache->get_all();
  EXPECT_EQ(sensor_profiles[0], updated_sensor_profile);
  EXPECT_EQ(sensor_profiles[1], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[0], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[1], updated_sensor_profile);
  EXPECT_NE(sensor_profiles[0], new_sensor_profile);
}

TEST(TestProfileCache, GetChangedSensorProfiles) {
  profile_cache_test new_test;
  wolf::sensor_profile new_sensor_profile;
  new_test.make_sensor_profile(new_sensor_profile, wolf::sensor_type::sht25,
                               wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->add(new_sensor_profile);
  wolf::sensor_profile another_new_sensor_profile;
  new_test.make_sensor_profile(another_new_sensor_profile,
                               wolf::sensor_type::enocean,
                               wolf::sensor_value_type::temperature, 2);
  new_test.new_cache->add(another_new_sensor_profile);
  wolf::profile_cache::get_all_result sensor_profiles;
  sensor_profiles = new_test.new_cache->get_all();
  EXPECT_EQ(sensor_profiles[0], new_sensor_profile);
  EXPECT_EQ(sensor_profiles[1], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[0], another_new_sensor_profile);
  EXPECT_NE(sensor_profiles[1], new_sensor_profile);
  wolf::sensor_id sensor_profile_id;
  sensor_profile_id =
      new_test.make_sensor_profile_id(wolf::sensor_value_type::temperature, 1);
  new_test.new_cache->remove(sensor_profile_id);
  sensor_profiles = new_test.new_cache->get_all();
  EXPECT_EQ(sensor_profiles.size(), 1);
}
