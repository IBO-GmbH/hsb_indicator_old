#ifndef WOLF_SENSOR_VALUE_OFFSET_HANDLER_HPP
#define WOLF_SENSOR_VALUE_OFFSET_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "wolf/sensor_value_offset_cache.hpp"

namespace wolf {

class sensor_value_offset_handler {
 public:
  sensor_value_offset_handler(sensor_value_offset_cache& database);

  void set(const sensor_value_offset offset);
  std::optional<float> get(const sensor_id& id) const;

  [[nodiscard]] float apply_offset_to_value(const sensor_id& id,
                                            const float value) const;

  boost::signals2::signal<void(const sensor_value_offset&,
                               const sensor_value_offset&)>
      signal_set;

 private:
  void load_from_cache();

  logging::logger m_logger;
  sensor_value_offset_cache& m_database;
  std::vector<sensor_value_offset> m_offsets;
};
}  // namespace wolf

#endif  // SENSOR_VALUE_OFFSET_HANDLER_HPP
