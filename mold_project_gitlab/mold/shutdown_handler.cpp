#include "shutdown_handler.hpp"

using namespace logging;

logger mold::shutdown_handler::m_logger("shutdown_handler");

void mold::shutdown_handler::shutdown() {
  LOGGING_LOG(m_logger, severity::normal) << "shutting down device!";
  std::system("/sbin/shutdown -h now");
}

void mold::shutdown_handler::reboot() {
  LOGGING_LOG(m_logger, severity::normal) << "rebooting device!";
  std::system("/sbin/shutdown -r now");
}
