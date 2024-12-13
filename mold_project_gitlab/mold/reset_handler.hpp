#ifndef MOLD_RESET_HANDLER_HPP
#define MOLD_RESET_HANDLER_HPP

#include <boost/signals2.hpp>
#include "log/logger.hpp"
#include "reset_mode.hpp"

namespace mold {

class reset_handler {
 public:
  reset_handler();
  void reset_values(const reset_mode& mode) const;
  void reset_config(const reset_mode& mode) const;

  boost::signals2::signal<void()> signal_reset_values;

 private:
  void shutdown_or_reboot(const reset_mode& mode) const;

 private:
  mutable logging::logger m_logger;
};
}  // namespace mold

#endif  // MOLD_RESET_HANDLER_HPP
