#include "mold/sensor_id_from_uuid_and_types.hpp"
#include "gtest/gtest.h"

TEST(SensorIdFromUuidAndTypes, Frsi) {
  const wolf::sensor_id check = mold::sensor_id_from_uuid_and_types::create(
      {0x01}, wolf::sensor_type::frsi, wolf::sensor_value_type::frsi);
  EXPECT_EQ(check.get_type(), wolf::sensor_value_type::frsi);
  EXPECT_LE(check.get_id().size(), 16);
}

TEST(SensorIdFromUuidAndTypes, FrsiNotSprout) {
  const wolf::sensor_id frsi = mold::sensor_id_from_uuid_and_types::create(
      {0x01}, wolf::sensor_type::frsi, wolf::sensor_value_type::frsi);
  const wolf::sensor_id sprout = mold::sensor_id_from_uuid_and_types::create(
      {0x01}, wolf::sensor_type::sprout_value, wolf::sensor_value_type::sprout);
  EXPECT_NE(frsi.get_id(), sprout.get_id());
  EXPECT_NE(frsi.get_type(), sprout.get_type());
  EXPECT_NE(frsi, sprout);
}
