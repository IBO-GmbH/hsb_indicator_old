#include "tone_enabled_handler.hpp"

#include "mold/options_cache.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;

tone_enabled_handler::tone_enabled_handler(options_cache* const database)
    : m_database(database), m_logger("tone_enabled_handler") {
  if (m_database == nullptr) {
    return;
  }
  m_enabled = m_database->get_tone_enabled().get_value_or(true);
}

void tone_enabled_handler::set_enabled(const bool enabled) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "mold::tone_enabled_handler " << enabled;
  wolf::thread_checker::throw_if_not_service(
      "tone_enabled_handler::set_enabled");
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (m_database != nullptr) {
    m_database->set_tone_enabled(enabled);
  }
  signal_changed(enabled);
}

bool tone_enabled_handler::is_enabled() { return m_enabled; }
