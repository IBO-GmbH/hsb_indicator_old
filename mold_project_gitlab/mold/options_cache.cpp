#include "options_cache.hpp"

#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

options_cache::options_cache(wolf::key_value_cache &cache)
    : m_logger{"options_cache"}, m_cache(cache) {}

void options_cache::set_building_type(const building_type &type) {
  wolf::thread_checker::throw_if_not_service(
      "options_cache::set_building_type");
  const int type_casted = static_cast<int>(type);
  m_cache.set("building_type", std::to_string(type_casted));
}

options_cache::optional_building options_cache::get_building_type() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_building_type";
  wolf::thread_checker::throw_if_not_service(
      "options_cache::get_building_type");
  const std::string building = m_cache.get("building_type");
  if (building.empty()) {
    return optional_building();
  }
  return static_cast<building_type>(std::stoi(building));
}

void options_cache::set_frsi(const float value) {
  wolf::thread_checker::throw_if_not_service("options_cache::set_frsi");
  m_cache.set("building_value", std::to_string(value));
}

options_cache::optional_float options_cache::get_frsi() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_frsi";
  const std::string value = m_cache.get("building_value");
  if (value.empty()) {
    return optional_float();
  }
  return std::stof(value);
}

void options_cache::set_substrate_type(const substrate_type &type) {
  wolf::thread_checker::throw_if_not_service(
      "options_cache::set_substrate_type");
  const int type_casted = static_cast<int>(type);
  m_cache.set("substrate_type", std::to_string(type_casted));
}

options_cache::optional_substrate options_cache::get_substrate_type() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_substrate_type";
  wolf::thread_checker::throw_if_not_service(
      "options_cache::get_substrate_type");
  const std::string substrate = m_cache.get("substrate_type");
  if (substrate.empty()) {
    return optional_substrate();
  }
  return static_cast<substrate_type>(std::stoi(substrate));
}

void options_cache::set_synced(const bool synced) {
  wolf::thread_checker::throw_if_not_service("options_cache::set_synced");
  if (synced) {
    m_cache.set("synced", "1");
    return;
  }
  m_cache.set("synced", "0");
}

options_cache::optional_bool options_cache::get_synced() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_synced";
  wolf::thread_checker::throw_if_not_service("options_cache::get_synced");
  const std::string synced_string = m_cache.get("synced");
  if (synced_string.empty()) {
    return optional_bool();
  }
  return synced_string == "1";
}

void options_cache::set_tone_enabled(const bool enabled) {
  wolf::thread_checker::throw_if_not_service("options_cache::set_tone_enabled");
  if (enabled) {
    m_cache.set("tone_enabled", "1");
    return;
  }
  m_cache.set("tone_enabled", "0");
}

options_cache::optional_bool options_cache::get_tone_enabled() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_tone_enabled";
  wolf::thread_checker::throw_if_not_service("options_cache::get_tone_enabled");
  const std::string tone_enabled = m_cache.get("tone_enabled");
  if (tone_enabled.empty()) {
    return optional_bool();
  }
  return tone_enabled == "1";
}

void options_cache::set_nightmode_enabled(const bool enabled) {
  wolf::thread_checker::throw_if_not_service(
      "options_cache::set_nightmode_enabled");
  if (enabled) {
    m_cache.set("nightmode_enabled", "1");
    return;
  }
  m_cache.set("nightmode_enabled", "0");
}

options_cache::optional_bool options_cache::get_nightmode_enabled() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_nightmode_enabled";
  wolf::thread_checker::throw_if_not_service(
      "options_cache::get_nightmode_enabled");
  const std::string nightmode_enabled = m_cache.get("nightmode_enabled");
  if (nightmode_enabled.empty()) return optional_bool();
  return nightmode_enabled == "1";
}

void options_cache::set_rotation_enabled(const bool enabled) {
  wolf::thread_checker::throw_if_not_service(
      "options_cache::set_rotation_enabled");
  if (enabled) {
    m_cache.set("rotation_enabled", "1");
    return;
  }
  m_cache.set("rotation_enabled", "0");
}

options_cache::optional_bool options_cache::get_rotation_enabled() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_rotation_enabled";
  wolf::thread_checker::throw_if_not_service(
      "options_cache::get_rotation_enabled");
  const std::string rotation_enabled = m_cache.get("rotation_enabled");
  if (rotation_enabled.empty()) {
    return optional_bool();
  }
  return rotation_enabled == "1";
}

void options_cache::set_pin(const std::string &pin) {
  wolf::thread_checker::throw_if_not_service("options_cache::set_pin");
  m_cache.set("protection_pin", pin);
}

options_cache::optional_string options_cache::get_pin() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_pin";
  wolf::thread_checker::throw_if_not_service("options_cache::get_pin");
  const std::string pin = m_cache.get("protection_pin");
  if (pin.empty()) {
    return optional_string();
  }
  return pin;
}
