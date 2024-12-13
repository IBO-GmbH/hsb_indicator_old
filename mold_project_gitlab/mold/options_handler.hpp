#ifndef MOLD_OPTIONS_HANDLER_HPP
#define MOLD_OPTIONS_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold/building_type.hpp"
#include "mold/options_cache.hpp"
#include "substrate_type.hpp"

namespace mold {

class options_handler {
 public:
  options_handler(options_cache& database, const float new_building,
                  const float old_building);
  virtual ~options_handler();

  void set_building_type_and_frsi(const building_type& type, const float value);
  void set_substrate_type(const substrate_type& type);
  void set_pin(const std::string& pin);
  void set_rotation_enabled(const bool enabled);

  virtual float get_frsi() const;
  building_type get_building_type() const;
  float get_building_value_new() const;
  float get_building_value_old() const;
  substrate_type get_substrate_type() const;
  const std::string get_pin() const;
  bool get_rotation_enabled() const;

  boost::signals2::signal<void()> signal_frsi_updated;
  boost::signals2::signal<void(const bool)> signal_rotation_enabled_changed;
  boost::signals2::signal<void()> signal_substrate_type_changed;

 private:
  logging::logger m_logger;

  options_cache& m_database;
  const float m_new_building;
  const float m_old_building;
  building_type m_building;
  substrate_type m_substrate;
  float m_frsi;
  std::string m_current_pin;
  bool m_rotation_enabled;
};
}  // namespace mold

#endif  // MOLD_OPTIONS_HANDLER_HPP
