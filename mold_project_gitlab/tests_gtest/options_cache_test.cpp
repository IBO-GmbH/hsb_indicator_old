#include "mold/options_cache.hpp"
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace options_cache_test {

using optional_building = boost::optional<mold::building_type>;
using optional_float = boost::optional<float>;
using optional_bool = boost::optional<bool>;
using optional_string = boost::optional<std::string>;

static logging::logger logger("options_cache_test");

struct options_cache_test {
  database_in_memory new_database;
  std::unique_ptr<mold::options_cache> new_cache;
  wolf::key_value_cache key_value_cache{new_database};
  options_cache_test() {
    new_cache.reset(new mold::options_cache(key_value_cache));
  }
  ~options_cache_test() { new_database.close(); }
};

TEST(TestOptionsCache, SetAndGetBuildingType) {
  options_cache_test new_test;
  EXPECT_NO_THROW(
      new_test.new_cache->set_building_type(mold::building_type::old));
  optional_building building;
  EXPECT_NO_THROW(building = new_test.new_cache->get_building_type());
  EXPECT_TRUE(building.has_value());
  EXPECT_EQ(building.get(), mold::building_type::old);
  EXPECT_NE(building.get(), mold::building_type::new_);
  EXPECT_NE(building.get(), mold::building_type::none);
  EXPECT_NO_THROW(
      new_test.new_cache->set_building_type(mold::building_type::new_));
  EXPECT_NO_THROW(building = new_test.new_cache->get_building_type());
  EXPECT_TRUE(building.has_value());
  EXPECT_EQ(building.get(), mold::building_type::new_);
  EXPECT_NE(building.get(), mold::building_type::old);
  EXPECT_NE(building.get(), mold::building_type::none);
  EXPECT_NO_THROW(
      new_test.new_cache->set_building_type(mold::building_type::none));
  EXPECT_NO_THROW(building = new_test.new_cache->get_building_type());
  EXPECT_TRUE(building.has_value());
  EXPECT_EQ(building.get(), mold::building_type::none);
  EXPECT_NE(building.get(), mold::building_type::new_);
  EXPECT_NE(building.get(), mold::building_type::old);
}

TEST(TestOptionsCache, SetAndGetBuildingValue) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_frsi(0.5f));
  optional_float building_value;
  EXPECT_NO_THROW(building_value = new_test.new_cache->get_frsi());
  EXPECT_TRUE(building_value.has_value());
  EXPECT_EQ(building_value.get(), 0.5f);
  EXPECT_NO_THROW(new_test.new_cache->set_frsi(0.7f));
  EXPECT_NO_THROW(building_value = new_test.new_cache->get_frsi());
  EXPECT_TRUE(building_value.has_value());
  EXPECT_EQ(building_value.get(), 0.7f);
}

TEST(TestOptionsCache, SetAndGetSyncedSynced) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_synced(true));
  optional_bool synced;
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  EXPECT_TRUE(synced.get());
}

TEST(TestOptionsCache, SetAndGetSyncedUnsynced) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_synced(false));
  optional_bool synced;
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  EXPECT_FALSE(synced.get());
}

TEST(TestOptionsCache, SetSyncedUnsynced) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_synced(true));
  optional_bool synced;
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  EXPECT_NO_THROW(new_test.new_cache->set_synced(false));
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  EXPECT_FALSE(synced.get());
}

TEST(TestOptionsCache, SetUnsyncedSynced) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_synced(false));
  optional_bool synced;
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  if (synced) {
    EXPECT_FALSE(synced.get());
  }
  EXPECT_NO_THROW(new_test.new_cache->set_synced(true));
  EXPECT_NO_THROW(synced = new_test.new_cache->get_synced());
  EXPECT_TRUE(synced.has_value());
  if (synced) {
    EXPECT_TRUE(synced.get());
  }
}

TEST(TestOptionsCache, SetAndGetToneEnabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetToneEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  EXPECT_FALSE(enabled.get());
}

TEST(TestOptionsCache, SetToneEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  EXPECT_TRUE(enabled.get());
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(false));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  if (enabled) {
    EXPECT_FALSE(enabled.get());
  }
}

TEST(TestOptionsCache, SetToneDisabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  if (enabled) {
    EXPECT_FALSE(enabled.get());
  }
  EXPECT_NO_THROW(new_test.new_cache->set_tone_enabled(true));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_tone_enabled());
  EXPECT_TRUE(enabled.has_value());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetNightmodeEnabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetNightmodeEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  EXPECT_FALSE(enabled.get());
}

TEST(TestOptionsCache, SetNightmodeEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  EXPECT_TRUE(enabled.get());
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(false));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  EXPECT_FALSE(enabled.get());
}

TEST(TestOptionsCache, SetNightmodeDisabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  if (enabled) {
    EXPECT_FALSE(enabled.get());
  }
  EXPECT_NO_THROW(new_test.new_cache->set_nightmode_enabled(true));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_nightmode_enabled());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetRotationEnabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetRotationEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  EXPECT_FALSE(enabled.get());
}

TEST(TestOptionsCache, SetRotationEnabledDisabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(true));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  EXPECT_TRUE(enabled.get());
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(false));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  EXPECT_FALSE(enabled.get());
}

TEST(TestOptionsCache, SetRotationDisabledEnabled) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(false));
  optional_bool enabled;
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  if (enabled) {
    EXPECT_FALSE(enabled.get());
  }
  EXPECT_NO_THROW(new_test.new_cache->set_rotation_enabled(true));
  EXPECT_NO_THROW(enabled = new_test.new_cache->get_rotation_enabled());
  EXPECT_TRUE(enabled.get());
}

TEST(TestOptionsCache, SetAndGetPin) {
  options_cache_test new_test;
  EXPECT_NO_THROW(new_test.new_cache->set_pin("1234"));
  optional_string pin;
  EXPECT_NO_THROW(pin = new_test.new_cache->get_pin());
  EXPECT_TRUE(pin.has_value());
  EXPECT_EQ(pin.get(), "1234");
  EXPECT_NO_THROW(new_test.new_cache->set_pin("5678"));
  EXPECT_NO_THROW(pin = new_test.new_cache->get_pin());
  EXPECT_TRUE(pin.has_value());
  EXPECT_EQ(pin.get(), "5678");
}
}  // namespace options_cache_test
