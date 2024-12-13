#ifndef WOLF_PROFILE_HANDLER_HPP
#define WOLF_PROFILE_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "wolf/mold_protocol_types.h"
#include "wolf/profile_cache.hpp"
#include "wolf/sensor_profile.hpp"

namespace wolf {

// TODO refactor to the style of the new protocol. save more. use more
class profile_handler {
 public:
  profile_handler(profile_cache& profile_cache);

  bool learn(const sensor_profile& profile);
  bool is_learned(const sensor_id& id);
  wolf::sensor_value_type get_sensor_value_type(const types::id& id,
                                                const types::id& owner) const;

  using learned = std::vector<sensor_profile>;
  const learned& get_all() const;
  sensor_profile get(const sensor_id& id) const;

  boost::signals2::signal<void(const sensor_profile& profile)> signal_update;

 private:
  void load_from_database();
  bool learn_internal(const sensor_profile& profile);
  learned::const_iterator find_profile(const sensor_id& id) const;
  learned::iterator find_profile(const sensor_id& id);

 private:
  logging::logger m_logger;
  profile_cache& m_database;
  learned m_profiles;
};
}  // namespace wolf

#endif  // WOLF_PROFILE_HANDLER_HPP
