#ifndef MOLD_CONFIGURATION_CACHE_HPP
#define MOLD_CONFIGURATION_CACHE_HPP

#include "mold/configuration.hpp"
#include "wolf/table_sync_helper.hpp"
#include "wolf/types.hpp"

namespace mold {

class configuration_cache {
 public:
  configuration_cache(wolf::database& database_);

  void add(const configuration& to_add);
  void update(const configuration& to_update);
  void remove(const wolf::types::uuid_array& id);

  using configurations = std::vector<configuration>;
  configurations get_all();

 private:
  logging::logger m_logger;

  using sync = wolf::table_sync_helper<configuration, std::string>;
  sync m_sync;
};
}  // namespace mold

#endif  // MOLD_CONFIGURATION_CACHE_HPP
