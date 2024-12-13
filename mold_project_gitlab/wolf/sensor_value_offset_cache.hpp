#ifndef WOLF_SENSOR_VALUE_OFFSET_CACHE_HPP
#define WOLF_SENSOR_VALUE_OFFSET_CACHE_HPP

#include "log/logger.hpp"
#include "wolf/database.hpp"
#include "wolf/sensor_value_offset.hpp"

namespace wolf {

class sensor_value_offset_cache {
 public:
  sensor_value_offset_cache(database &database_);

  void set(const sensor_value_offset &value);
  using sensor_value_offsets = std::vector<sensor_value_offset>;
  sensor_value_offsets get_all();

 private:
  void create_tables();

  logging::logger m_logger;
  soci::session &m_session;
};
}  // namespace wolf
#endif  // WOLF_SENSOR_VALUE_OFFSET_CACHE_HPP
