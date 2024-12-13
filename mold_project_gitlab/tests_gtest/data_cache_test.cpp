#include "wolf/value_cache.hpp"
#include "gtest/gtest.h"
#include "wolf/database.hpp"

namespace data_cache_test {

struct data_cache_test {
  wolf::database new_database;
  std::unique_ptr<wolf::value_cache> new_cache;
  data_cache_test() {
    new_database.open(":memory:");
    new_cache.reset(new wolf::value_cache(new_database, true));
  }
  void make_new_data(wolf::types::uuid_array& work_uuid, unsigned char id,
                     wolf::value_cache::data& cache_data,
                     wolf::types::id sensor_id, wolf::types::data data,
                     std::uint64_t timestamp, int rssi, bool did_send) {
    make_work_uuid(work_uuid, id);
    cache_data.sensor_id = sensor_id;
    cache_data.data = data;
    cache_data.timestamp = timestamp;
    cache_data.rssi = rssi;
    cache_data.did_send = did_send;
  }
  void make_work_uuid(wolf::types::uuid_array& work_uuid, unsigned char id) {
    std::fill(work_uuid.begin(), work_uuid.end(), id);
  }
  ~data_cache_test() { new_database.close(); }
};

TEST(TestDataCache, AddNewDataNoError) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data, 22, 34,
                         true);
  EXPECT_NO_THROW(new_test.new_cache->save_data(
      work_uuid, new_cache_data.sensor_id, new_cache_data.data,
      new_cache_data.timestamp, &new_cache_data.rssi, new_cache_data.did_send));
}

TEST(TestDataCache, SetDataDoneNoError) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data, 22, 34,
                         false);
  new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                new_cache_data.data, new_cache_data.timestamp,
                                &new_cache_data.rssi, new_cache_data.did_send);
  EXPECT_NO_THROW(new_test.new_cache->set_data_done(work_uuid, true));
}

TEST(TestDataCache, GetUndoneDataNoUndone) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data, 22, 34,
                         true);
  new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                new_cache_data.data, new_cache_data.timestamp,
                                &new_cache_data.rssi, new_cache_data.did_send);
  new_test.new_cache->set_data_done(work_uuid, true);
  std::vector<wolf::value_cache::data> undone_data;
  EXPECT_NO_THROW(undone_data =
                      new_test.new_cache->get_undone_data(work_uuid, 5));
  EXPECT_EQ(undone_data.size(), 0);
}

TEST(TestDataCache, GetUndoneDataUndone) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data, 22, 34,
                         false);
  new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                new_cache_data.data, new_cache_data.timestamp,
                                &new_cache_data.rssi, new_cache_data.did_send);
  std::vector<wolf::value_cache::data> undone_data;
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 1);
  new_test.new_cache->set_data_done(work_uuid, true);
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 0);
}

TEST(TestDataCache, GetUndoneDataResetUUIDs) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data, 22, 34,
                         false);
  new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                new_cache_data.data, new_cache_data.timestamp,
                                &new_cache_data.rssi, new_cache_data.did_send);
  std::vector<wolf::value_cache::data> undone_data;
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 1);
  new_test.new_cache->set_data_done(work_uuid, true);
  new_test.new_cache->reset_work_uuids();
  new_test.make_work_uuid(work_uuid, 2);
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 0);
}

TEST(TestDataCache, GetUndoneDataUndoneMoreElementsSameID) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  for (int index = 0; index <= 4; ++index) {
    new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data,
                           index + 10, index + 20, false);
    new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                  new_cache_data.data, new_cache_data.timestamp,
                                  &new_cache_data.rssi,
                                  new_cache_data.did_send);
  }
  std::vector<wolf::value_cache::data> undone_data;
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 5);
  new_test.new_cache->set_data_done(work_uuid, true);
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 0);
}

TEST(TestDataCache, GetUndoneDataResetUUIDsMoreElementsSameID) {
  data_cache_test new_test;
  wolf::types::uuid_array work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  for (unsigned int index = 0; index <= 4; ++index) {
    new_test.make_new_data(work_uuid, 1, new_cache_data, new_id, new_data,
                           index + 10, index + 20, false);
    new_test.new_cache->save_data(work_uuid, new_cache_data.sensor_id,
                                  new_cache_data.data, new_cache_data.timestamp,
                                  &new_cache_data.rssi,
                                  new_cache_data.did_send);
  }
  std::vector<wolf::value_cache::data> undone_data;
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 5);
  new_test.new_cache->set_data_done(work_uuid, true);
  new_test.new_cache->reset_work_uuids();
  new_test.make_work_uuid(work_uuid, 2);
  undone_data = new_test.new_cache->get_undone_data(work_uuid, 5);
  EXPECT_EQ(undone_data.size(), 0);
}

TEST(TestDataCache, GetUndoneDataResetUUIDsMoreElementsDifferentIDsNoDone) {
  data_cache_test new_test;
  std::array<wolf::types::uuid_array, 6> work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  for (unsigned int index = 0; index <= 4; ++index) {
    new_test.make_new_data(work_uuid[index], index, new_cache_data, new_id,
                           new_data, index + 10, index + 20, false);
    new_test.new_cache->save_data(work_uuid[index], new_cache_data.sensor_id,
                                  new_cache_data.data, new_cache_data.timestamp,
                                  &new_cache_data.rssi,
                                  new_cache_data.did_send);
  }
  std::vector<wolf::value_cache::data> undone_data;
  for (unsigned int index = 0; index <= 4; ++index) {
    undone_data = new_test.new_cache->get_undone_data(work_uuid[index], 5);
    EXPECT_EQ(undone_data.size(), 1);
  }
  new_test.new_cache->reset_work_uuids();
  new_test.make_work_uuid(work_uuid[5], 6);
  undone_data = new_test.new_cache->get_undone_data(work_uuid[5], 5);
  EXPECT_EQ(undone_data.size(), 5);
}

TEST(TestDataCache, GetUndoneDataResetUUIDsMoreElementsDifferentIDsOneDone) {
  data_cache_test new_test;
  std::array<wolf::types::uuid_array, 6> work_uuid;
  wolf::value_cache::data new_cache_data;
  wolf::types::id new_id;
  wolf::types::data new_data;
  new_id.push_back(45);
  new_data.push_back(23);
  for (unsigned int index = 0; index <= 4; ++index) {
    new_test.make_new_data(work_uuid[index], index, new_cache_data, new_id,
                           new_data, index + 10, index + 20, false);
    new_test.new_cache->save_data(work_uuid[index], new_cache_data.sensor_id,
                                  new_cache_data.data, new_cache_data.timestamp,
                                  &new_cache_data.rssi,
                                  new_cache_data.did_send);
  }
  std::vector<wolf::value_cache::data> undone_data;
  for (unsigned int index = 0; index <= 4; ++index) {
    undone_data = new_test.new_cache->get_undone_data(work_uuid[index], 5);
    EXPECT_EQ(undone_data.size(), 1);
  }
  new_test.new_cache->set_data_done(work_uuid[2], true);
  undone_data = new_test.new_cache->get_undone_data(work_uuid[2], 5);
  EXPECT_EQ(undone_data.size(), 0);
  new_test.new_cache->reset_work_uuids();
  new_test.make_work_uuid(work_uuid[5], 6);
  undone_data = new_test.new_cache->get_undone_data(work_uuid[5], 5);
  EXPECT_EQ(undone_data.size(), 4);
}
}
