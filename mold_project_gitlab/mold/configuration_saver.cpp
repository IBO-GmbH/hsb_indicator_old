#include "configuration_saver.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_generator.hpp"

using namespace mold;

configuration_saver::configuration_saver(
    configuration_handler &configuration_handler_)
    : m_logger{"configuration_saver"},
      m_configuration_handler(configuration_handler_) {}

configuration configuration_saver::save_and_update(
    configuration configuration_) {
  wolf::thread_checker::is_service();

  const bool isAdd = configuration_.id == wolf::types::uuid_array{};
  const bool temperatureSet = configuration_.temperature != wolf::sensor_id();
  const bool humiditySet = configuration_.humidity != wolf::sensor_id();
  if (!temperatureSet) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "no indoor temperature sensor got set!" << configuration_;
  }
  if (!humiditySet) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "no indoor humidity sensor got set!" << configuration_;
  }
  if (!temperatureSet && !humiditySet) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "no indoor sensors got set!" << configuration_;
  }
  if (isAdd) {
    configuration_.id = wolf::uuid_generator::generate_as_array();
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "saving new, config:" << configuration_;
    m_configuration_handler.add(configuration_);
    return configuration_;
  }
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "updating, config:" << configuration_;
  m_configuration_handler.update(configuration_);
  return configuration_;
}
