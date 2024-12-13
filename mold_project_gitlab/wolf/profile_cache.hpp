#ifndef WOLF_LEARN_CACHE_HPP
#define WOLF_LEARN_CACHE_HPP

#include "wolf/sensor_profile.hpp"
#include "wolf/table_sync_helper.hpp"

namespace soci {
class session;
}

namespace wolf {

class profile_cache {
 public:
  profile_cache(wolf::database& database_);

  void add(const sensor_profile& to_add);
  void update(const sensor_profile& to_update);
  void remove(const sensor_id& id);
  using get_all_result = std::vector<sensor_profile>;
  get_all_result get_all();

 private:
  logging::logger m_logger;
  using sync = table_sync_helper<sensor_profile, std::string>;
  sync m_sync;
};
}  // namespace wolf

#endif  // WOLF_LEARN_CACHE_HPP
