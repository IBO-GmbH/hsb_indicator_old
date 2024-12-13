#include "options_handler.hpp"
#include <boost/optional/optional_io.hpp>
#include "mold/options_cache.hpp"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_generator.hpp"

using namespace logging;
using namespace mold;

options_handler::options_handler(options_cache &database,
                                 const float new_building,
                                 const float old_building)
    : m_logger("options_handler"),
      m_database(database),
      m_new_building(new_building),
      m_old_building(old_building),
      m_building{building_type::new_},
      m_substrate(substrate_type::type_two),
      m_frsi{m_new_building},
      m_current_pin(""),
      m_rotation_enabled(true) {
  auto building = m_database.get_building_type();
  if (building) {
    m_building = building.get();
    LOGGING_LOG(m_logger, severity::normal) << "successfully loaded,"
                                               " m_building: "
                                            << m_building;
  }
  auto value = m_database.get_frsi();
  if (value) {
    m_frsi = value.get();
    LOGGING_LOG(m_logger, severity::normal) << "successfully loaded,"
                                               " m_frsi: "
                                            << m_frsi;
  }
  auto substrate = m_database.get_substrate_type();
  if (substrate) {
    m_substrate = substrate.get();
    LOGGING_LOG(m_logger, severity::normal) << "successfully loaded,"
                                               " m_substrate: "
                                            << m_substrate;
  }
  auto saved_pin = m_database.get_pin();
  if (saved_pin) {
    m_current_pin = saved_pin.get();
    LOGGING_LOG(m_logger, severity::normal) << "successfully loaded,"
                                               " m_current_pin: "
                                            << m_current_pin;
  }
  auto rotation = m_database.get_rotation_enabled();
  if (rotation) {
    m_rotation_enabled = rotation.get();
    LOGGING_LOG(m_logger, severity::normal) << "successfully loaded,"
                                               " m_rotation_enabled: "
                                            << m_rotation_enabled;
  }
}

options_handler::~options_handler() {}

void options_handler::set_building_type_and_frsi(const building_type &type,
                                                 const float value) {
  wolf::thread_checker::throw_if_not_service(
      "options_handler::set_building_type_and_frsi");

  LOGGING_LOG(m_logger, severity::normal)
      << "set_new_building, type:" << type << " frsi:" << value;

  if (m_building == type && wolf::math::compare_floats(m_frsi, value)) {
    // nothing changed
    LOGGING_LOG(m_logger, severity::verbose) << "nothing changed";
    return;
  }

  m_building = type;
  m_frsi = value;

  m_database.set_building_type(m_building);
  m_database.set_frsi(m_frsi);
  m_database.set_synced(false);
  signal_frsi_updated();
}

void options_handler::set_substrate_type(const substrate_type &type) {
  wolf::thread_checker::throw_if_not_service(
      "options_handler::set_substrate_type");

  LOGGING_LOG(m_logger, severity::normal)
      << "set_substrate_type, type:" << type;

  if (m_substrate == type) {
    // nothing changed
    LOGGING_LOG(m_logger, severity::verbose) << "nothing changed";
    return;
  }

  m_substrate = type;

  m_database.set_substrate_type(m_substrate);
  m_database.set_synced(false);
  signal_substrate_type_changed();
}

void options_handler::set_pin(const std::string &pin) {
  wolf::thread_checker::throw_if_not_service("options_handler::set_pin");
  LOGGING_LOG(m_logger, severity::normal) << "set_pin, pin:" << pin;

  if (m_current_pin == pin) {
    // nothing changed
    LOGGING_LOG(m_logger, severity::verbose) << "nothing changed";
    return;
  }

  m_current_pin = pin;
  m_database.set_pin(pin);
}

void options_handler::set_rotation_enabled(const bool enabled) {
  wolf::thread_checker::throw_if_not_service(
      "options_handler::set_rotation_enabled");
  LOGGING_LOG(m_logger, severity::normal)
      << "set_rotation_enabled, enabled:" << enabled;

  if (m_rotation_enabled == enabled) {
    // nothing changed
    LOGGING_LOG(m_logger, severity::verbose) << "nothing changed";
    return;
  }

  m_rotation_enabled = enabled;
  m_database.set_rotation_enabled(enabled);
  signal_rotation_enabled_changed(enabled);
}

float options_handler::get_frsi() const { return m_frsi; }

building_type options_handler::get_building_type() const { return m_building; }

float options_handler::get_building_value_new() const { return m_new_building; }

float options_handler::get_building_value_old() const { return m_old_building; }

substrate_type options_handler::get_substrate_type() const {
  return m_substrate;
}

const std::string options_handler::get_pin() const { return m_current_pin; }

bool options_handler::get_rotation_enabled() const {
  return m_rotation_enabled;
}
