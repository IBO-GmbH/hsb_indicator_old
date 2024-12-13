#ifndef WOLF_ENOCEAN_HANDLER_HPP
#define WOLF_ENOCEAN_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "mold/configuration_values_handler.hpp"
#include "wolf/enocean_cache.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace wolf {

class enocean_link;

class enocean_handler {
 public:
  enocean_handler(enocean_cache &cache);
  ~enocean_handler();

  std::vector<wolf::sensor_profile> register_(const types::id_esp3 id,
                                              const types::eep eep,
                                              const bool is_outdoor);
  void handle_value(const types::id_esp3 id, const types::data &data,
                    const int rssi, const bool is_outdoor);
  void handle_value(const types::id_esp3 id, const types::data &data,
                    const int rssi,
                    const std::chrono::system_clock::time_point &time_stamp,
                    const bool is_outdoor);
  void remove_devices(const std::vector<types::id_esp3> &ids);

  boost::signals2::signal<void(const std::vector<sensor_value> &,
                               const int rssi)>
      signal_values;
  boost::signals2::signal<void(const std::vector<sensor_profile> &)>
      signal_profiles_loaded;

 private:
  void load();

  logging::logger m_logger;
  enocean_cache &m_cache;
  std::unique_ptr<enocean_link> m_enocean_link;
};
}  // namespace wolf

#endif
