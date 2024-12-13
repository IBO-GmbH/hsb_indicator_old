#ifndef MOLD_SPROUT_CACHE_HPP
#define MOLD_SPROUT_CACHE_HPP

#include "mold/sprout/data_types.hpp"
#include "wolf/key_value_cache.hpp"

namespace mold::sprout {

class cache {
 public:
  cache(wolf::key_value_cache& key_value_database);

  void save_all(const data_types::values& values);
  data_types::values load_all();

 private:
  logging::logger m_logger;
  wolf::key_value_cache& m_key_value_database;
};
}  // namespace mold::sprout

#endif
