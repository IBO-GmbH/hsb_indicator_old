#ifndef MOLD_SHUTDOWN_HANDLER_HPP
#define MOLD_SHUTDOWN_HANDLER_HPP

#include "log/logger.hpp"

namespace mold {

class shutdown_handler {
 public:
  static void shutdown();
  static void reboot();

 private:
  static logging::logger m_logger;
};
}  // namespace mold

#endif  // MOLD_SHUTDOWN_HANDLER_HPP
