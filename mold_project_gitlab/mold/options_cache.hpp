#ifndef MOLD_OPTIONS_CACHE_HPP
#define MOLD_OPTIONS_CACHE_HPP

#include "mold/building_type.hpp"
#include "mold/substrate_type.hpp"
#include "wolf/key_value_cache.hpp"

namespace mold {

class options_cache {
 public:
  using optional_building = boost::optional<building_type>;
  using optional_float = boost::optional<float>;
  using optional_bool = boost::optional<bool>;
  using optional_string = boost::optional<std::string>;
  using optional_substrate = boost::optional<substrate_type>;

  options_cache(wolf::key_value_cache &cache);

  void set_building_type(const building_type &type);
  optional_building get_building_type();
  void set_frsi(const float value);
  optional_float get_frsi();
  void set_substrate_type(const substrate_type &type);
  optional_substrate get_substrate_type();

  void set_synced(const bool synced);
  optional_bool get_synced();

  void set_tone_enabled(const bool enabled);
  optional_bool get_tone_enabled();

  void set_nightmode_enabled(const bool enabled);
  optional_bool get_nightmode_enabled();

  void set_rotation_enabled(const bool enabled);
  optional_bool get_rotation_enabled();

  void set_pin(const std::string &pin);
  optional_string get_pin();

 private:
  logging::logger m_logger;
  wolf::key_value_cache &m_cache;
};
}  // namespace mold

#endif  // MOLD_OPTIONS_CACHE_HPP
