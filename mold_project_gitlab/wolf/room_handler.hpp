#ifndef WOLF_ROOM_HANDLER_HPP
#define WOLF_ROOM_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "wolf/mold_protocol_types.h"

#include "wolf/room.hpp"
#include "wolf/room_cache.hpp"

namespace wolf {

class room_handler {
 public:
  room_handler(room_cache &database, const bool rssi_enabled);

  virtual bool add(const room &to_add);
  bool update(const room &to_update);
  bool remove(const types::uuid_array &id);
  using rooms = std::vector<room>;
  const rooms &get_all() const;
  const room &get(const types::uuid_array &id) const;
  const rooms get_all_rooms_with_sensor(const sensor_id &id) const;

  void replace_one_or_add_sensor_in_all(const sensor_id &old,
                                        const sensor_id &new_);
  using sensors = std::vector<sensor_id>;
  void remove_unused_sensors(const sensors &sensor_list);
  virtual ~room_handler() = default;

  boost::signals2::signal<void(const room &)> signal_added;
  boost::signals2::signal<void(const room &)> signal_updated;
  boost::signals2::signal<void(const types::uuid_array)> signal_removed;
  boost::signals2::signal<void(const sensors)> signal_remove_values_for_sensors;
  using sensors_vector = std::vector<types::id_esp3>;
  boost::signals2::signal<void(const sensors_vector &)> signal_remove_sensors;

 private:
  rooms::const_iterator find(const types::uuid_array &id) const;
  rooms::iterator find(const types::uuid_array &id);
  using sensors_set = std::set<types::id_esp3>;
  static sensors_set generate_enocean_sensor_list(const rooms &rooms_);
  static sensors::const_iterator find_enocean_sensor(const room &room_);
  static types::id_esp3 convert_sensor_id(const sensor_id &id);
  void remove_unused_sensors(const sensors_set &sensor_list);
  static bool has_rssi_sensor(const room &room_);
  static bool is_sensor_in_room(const room &room_, const sensor_id &sensor);

 private:
  mutable logging::logger m_logger;
  room_cache &m_database;
  rooms m_rooms;
};
}  // namespace wolf

#endif  // WOLF_ROOM_HANDLER_HPP
