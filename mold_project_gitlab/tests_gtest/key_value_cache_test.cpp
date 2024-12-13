#include "wolf/key_value_cache.hpp"
#include <boost/optional/optional_io.hpp>
#include "database_in_memory.hpp"
#include "gtest/gtest.h"
#include "wolf/sensor_id.hpp"
#include "wolf/sensor_type.hpp"
#include "wolf/types.hpp"

class TestKeyValueCache : public ::testing::Test {
 protected:
  database_in_memory new_database;
  wolf::key_value_cache new_cache{new_database};
};

TEST_F(TestKeyValueCache, SetAndGetOptions) {
  new_cache.set("new option", "option value");
  new_cache.set("new option 2", "option value 2");
  EXPECT_EQ(new_cache.get("new option"), "option value");
  EXPECT_NE(new_cache.get("new option 2"), "option value");
  new_database.close();
}

TEST_F(TestKeyValueCache, SetAndGetLanguage) {
  new_cache.set_language("new language");
  EXPECT_EQ(new_cache.get_language(), "new language");
  EXPECT_NE(new_cache.get_language(), "old language");
  new_cache.set_language("new language 2");
  EXPECT_EQ(new_cache.get_language(), "new language 2");
  EXPECT_NE(new_cache.get_language(), "new language");
  new_database.close();
}

TEST_F(TestKeyValueCache, SetAndGetDatabaseID) {
  wolf::types::uuid_array database_id;
  for (unsigned int index = 0; index < 16; ++index) database_id[index] = 1;
  EXPECT_FALSE(new_cache.get_database_id(database_id));
  new_cache.set_database_id(database_id);
  EXPECT_TRUE(new_cache.get_database_id(database_id));
  new_database.close();
}

TEST_F(TestKeyValueCache, SetAndGetOutdoorSensor) {
  wolf::sensor_id sensor_id({'a'}, wolf::sensor_value_type::temperature);
  wolf::sensor_id sensor_id_second({'a'}, wolf::sensor_value_type::humidity);
  new_cache.set_outdoor_sensor(sensor_id);
  EXPECT_EQ(new_cache.get_outdoor_sensor(), sensor_id);
  EXPECT_NE(new_cache.get_outdoor_sensor(), sensor_id_second);
  new_cache.set_outdoor_sensor(sensor_id_second);
  EXPECT_EQ(new_cache.get_outdoor_sensor(), sensor_id_second);
  EXPECT_NE(new_cache.get_outdoor_sensor(), sensor_id);
  new_database.close();
}

TEST_F(TestKeyValueCache, SetAndGetOutdoorSensorHumidity) {
  wolf::sensor_id sensor_id({'a'}, wolf::sensor_value_type::humidity);
  wolf::sensor_id sensor_id_second({'b'}, wolf::sensor_value_type::humidity);
  new_cache.set_outdoor_sensor_humidity(sensor_id);
  EXPECT_EQ(new_cache.get_outdoor_sensor_humidity(), sensor_id);
  EXPECT_NE(new_cache.get_outdoor_sensor_humidity(), sensor_id_second);
  new_cache.set_outdoor_sensor_humidity(sensor_id_second);
  EXPECT_EQ(new_cache.get_outdoor_sensor_humidity(), sensor_id_second);
  EXPECT_NE(new_cache.get_outdoor_sensor_humidity(), sensor_id);
  new_database.close();
}

TEST_F(TestKeyValueCache, SetAndGetOutdoorSensorSynced) {
  new_cache.set_outdoor_sensor_synced(true);
  EXPECT_TRUE(new_cache.get_outdoor_sensor_synced());
  new_cache.set_outdoor_sensor_synced(false);
  EXPECT_FALSE(new_cache.get_outdoor_sensor_synced());
  new_database.close();
}
