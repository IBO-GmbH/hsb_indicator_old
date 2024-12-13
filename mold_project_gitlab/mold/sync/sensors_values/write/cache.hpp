#ifndef MOLD_SYNC_SENSORS_VALUES_WRITE_CACHE_HPP
#define MOLD_SYNC_SENSORS_VALUES_WRITE_CACHE_HPP

#include "wolf/database.hpp"
#include "wolf/sensor_value.hpp"

namespace mold::sync::sensors_values::write {

class cache {
 public:
  cache(wolf::database& database);

  void add(const wolf::sensor_value& value);
  using key = int;
  struct entry {
    key key_;
    wolf::sensor_value value;
  };
  std::vector<entry> get_some();
  void remove(const std::vector<key>& keys);
  void clear_all_values_and_block_new_ones();
  void remove_all_values_for_sensor_id(const wolf::sensor_id& id);

 private:
  void create_tables();

  logging::logger m_logger;
  soci::session& m_session;

  bool m_block;
};

}  // namespace mold::sync::sensors_values::write

#endif
