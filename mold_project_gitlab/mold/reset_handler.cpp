#include "reset_handler.hpp"
#include "shutdown_handler.hpp"

using namespace logging;

mold::reset_handler::reset_handler() : m_logger("reset_handler") {}

void mold::reset_handler::reset_values(const reset_mode &mode) const {
  LOGGING_LOG(m_logger, severity::normal) << "reset sensor values and " << mode;
  signal_reset_values();
  shutdown_or_reboot(mode);
}

void mold::reset_handler::reset_config(const reset_mode &mode) const {
  LOGGING_LOG(m_logger, severity::normal)
      << "reset device config and sensor values and " << mode;
  std::system("./reset_device");
  shutdown_or_reboot(mode);
}

void mold::reset_handler::shutdown_or_reboot(
    const mold::reset_mode &mode) const {
  switch (mode) {
    case reset_mode::shutdown:
      shutdown_handler::shutdown();
      return;
    case reset_mode::reboot:
      shutdown_handler::reboot();
      return;
  }
  LOGGING_LOG(m_logger, severity::error)
      << "mode was neither shutdown nor reboot!";
}
