#ifndef MOLD_SYNC_BOX_SETTINGS_CACHE_HPP
#define MOLD_SYNC_BOX_SETTINGS_CACHE_HPP

#include "wolf/database.hpp"
#include "wolf/types.hpp"

namespace mold::sync::box_settings {

class cache {
 public:
  cache(wolf::database& database);
  void mark_dirty();
  void synchronized(const int version);
  void raise_version();
  struct dirty {
    bool is_dirty{};
    int version{};
  };
  dirty get();

 private:
  void create_tables();
  void set(const dirty to_set);

  logging::logger m_logger;
  soci::session& m_session;
};

}  // namespace mold::sync::box_settings
#endif
