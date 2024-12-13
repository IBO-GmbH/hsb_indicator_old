#include "gtest/gtest.h"
#include "wolf/sensor_value_offset_handler.hpp"

struct SensorValueOffset : ::testing::Test {
  wolf::database database{":memory:"};
  wolf::sensor_value_offset_cache cache{database};
  wolf::sensor_value_offset_handler handler{cache};
};

static wolf::sensor_id create_sensor_id() {
  return wolf::sensor_id({'a'}, wolf::sensor_value_type::temperature);
}

static wolf::sensor_id create_another_sensor_id() {
  return wolf::sensor_id({'a'}, wolf::sensor_value_type::humidity);
}

struct insert_a_value {
  const wolf::sensor_id id = create_sensor_id();
  const float offset = 1.23f;
  insert_a_value(wolf::sensor_value_offset_handler &handler) {
    handler.set({id, offset});
  }
};
struct insert_another_value {
  const wolf::sensor_id id = create_another_sensor_id();
  const float offset = 4.32f;
  insert_another_value(wolf::sensor_value_offset_handler &handler) {
    handler.set({id, offset});
  }
};

TEST_F(SensorValueOffset, SetGet) {
  insert_a_value inserter{handler};
  EXPECT_EQ(handler.get(inserter.id), inserter.offset);
}

TEST_F(SensorValueOffset, SetSetGet) {
  insert_a_value inserter{handler};
  const auto another_offset = 4.32f;
  handler.set({inserter.id, another_offset});
  EXPECT_EQ(handler.get(inserter.id), another_offset);
}

TEST_F(SensorValueOffset, SetGetInvalid) {
  insert_a_value inserter{handler};
  const auto id = create_another_sensor_id();
  EXPECT_FALSE(handler.get(id));
}

TEST_F(SensorValueOffset, SetApplyOffset) {
  insert_a_value inserter{handler};
  const float value = 2.f;
  const float result = handler.apply_offset_to_value(inserter.id, 2.f);
  EXPECT_FLOAT_EQ(value + inserter.offset, result);
}

TEST_F(SensorValueOffset, ApplyOffsetToNotSet) {
  const auto id = create_sensor_id();
  const float value = 2.f;
  const float result = handler.apply_offset_to_value(id, 2.f);
  EXPECT_FLOAT_EQ(value, result);
}

TEST_F(SensorValueOffset, SetGetDatabase) {
  EXPECT_TRUE(cache.get_all().empty());
  insert_a_value inserter{handler};
  const auto all = cache.get_all();
  EXPECT_EQ(all.size(), 1);
  const auto &first = all.front();
  EXPECT_EQ(first.id, inserter.id);
  EXPECT_EQ(first.offset, inserter.offset);
}

TEST_F(SensorValueOffset, SetSetGetDatabase) {
  insert_a_value inserter{handler};
  insert_another_value another_inserter{handler};
  const auto all = cache.get_all();
  EXPECT_EQ(all.size(), 2);
}

TEST_F(SensorValueOffset, SetUpdateGetDatabase) {
  insert_a_value inserter{handler};
  const auto another_offset = 4.32f;
  handler.set({inserter.id, another_offset});
  const auto all = cache.get_all();
  EXPECT_EQ(all.size(), 1);
  const auto &first = all.front();
  EXPECT_EQ(first.id, inserter.id);
  EXPECT_EQ(first.offset, another_offset);
}

TEST_F(SensorValueOffset, LoadAll) {
  insert_a_value inserter{handler};
  wolf::sensor_value_offset_cache cache2{database};
  wolf::sensor_value_offset_handler handler2{cache2};
  const auto offset = handler2.get(inserter.id);
  EXPECT_TRUE(offset);
  EXPECT_FLOAT_EQ(offset.value(), inserter.offset);
}
