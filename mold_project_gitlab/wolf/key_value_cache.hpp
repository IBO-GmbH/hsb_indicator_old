#ifndef WOLF_KEY_VALUE_CACHE_HPP
#define WOLF_KEY_VALUE_CACHE_HPP

#include <boost/optional/optional.hpp>
#include "database_cache.hpp"
#include "log/logger.hpp"
#include "wolf/sensor_id.hpp"
#include "wolf/types.hpp"

namespace wolf {
class key_value_cache : public database_cache {
 public:
  using base = wolf::database_cache;

  key_value_cache(wolf::database& database_);

  void set(const std::string& key, const std::string& value);
  std::string get(const std::string& key);

  void set_language(const std::string& language);
  std::string get_language();

  void set_database_id(const wolf::types::uuid_array& id);
  bool get_database_id(wolf::types::uuid_array& fill);

  void set_outdoor_sensor(const wolf::sensor_id& id);
  boost::optional<sensor_id> get_outdoor_sensor();
  void set_outdoor_sensor_humidity(const wolf::sensor_id& id);
  boost::optional<sensor_id> get_outdoor_sensor_humidity();
  void set_outdoor_sensor_synced(const bool& synced);
  bool get_outdoor_sensor_synced();

 private:
  void create_tables();
  logging::logger m_logger;
};
}  // namespace wolf

#endif  // WOLF_KEY_VALUE_CACHE_HPP
