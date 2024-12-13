#ifndef MOLD_MQTT_CACHE_HPP
#define MOLD_MQTT_CACHE_HPP

#include "wolf/table_sync_helper.hpp"
#include "wolf/types.hpp"

namespace mold {

class mqtt_cache {
 public:
  struct mqtt_client {
    std::string client_id;
    wolf::types::uuid_array room_id;
  };

  mqtt_cache(wolf::database& database_);
  ~mqtt_cache() = default;
  void add(const mqtt_client& to_add);
  void remove(const std::string& to_remove);

  using clients = std::vector<mqtt_client>;
  clients get_all();

 protected:
  logging::logger m_logger;
  using sync = wolf::table_sync_helper<mqtt_client, std::string>;
  sync m_sync;
};
}  // namespace mold

#endif  // MOLD_MQTT_CACHE_HPP
