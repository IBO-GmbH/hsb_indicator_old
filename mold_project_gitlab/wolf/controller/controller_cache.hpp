#ifndef WOLF_CONTROLLER_CONTROLLER_CACHE_HPP
#define WOLF_CONTROLLER_CONTROLLER_CACHE_HPP

#include "log/logger.hpp"
#include "values_per_room.hpp"
#include "wolf/database_cache.hpp"

namespace wolf::controller {

class controller_cache : public database_cache {
 public:
  controller_cache(database& database_);
  virtual bool add(const values_per_room& to_add);
  virtual bool update(const values_per_room& to_update);
  virtual bool remove(const types::uuid_array& room_id);

  using values_list = std::vector<values_per_room>;
  virtual values_list get_all();

  virtual ~controller_cache() = default;

 protected:
  void write(const std::string& key, const std::string& value);
  std::string get(const std::string& key);

 private:
  void create_tables();
  values_list::iterator find(const types::uuid_array& room_id);
  void save_all();
  void load_all();

 private:
  logging::logger m_logger;
  // TODO *_cache classes must not have a state!
  values_list m_values;
};
}  // namespace wolf::controller

#endif  // WOLF_CONTROLLER_CONTROLLER_CACHE_HPP
