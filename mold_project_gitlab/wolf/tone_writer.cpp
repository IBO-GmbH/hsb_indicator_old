#include "tone_writer.hpp"

#include <wiringPi.h>

using namespace logging;

using namespace wolf;

tone_writer::tone_writer(const int &gpio)
    : m_logger{"tone_writer"}, m_gpio{gpio}, m_enabled{false} {
  ;
}

bool tone_writer::open() {
  LOGGING_LOG(m_logger, severity::normal)
      << "opening tone on gpio:" << std::dec << m_gpio;
  pinMode(m_gpio, OUTPUT);
  return true;
}

void tone_writer::set_enabled(const bool &enabled) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "enabling tone, enabled:" << std::boolalpha << enabled
      << " m_enabled:" << m_enabled;
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (enabled) {
    digitalWrite(m_gpio, HIGH);
    return;
  }
  digitalWrite(m_gpio, LOW);
}
