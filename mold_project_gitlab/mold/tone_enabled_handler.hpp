#ifndef MOLD_TONE_ENABLED_HANDLER_HPP
#define MOLD_TONE_ENABLED_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold/options_cache.hpp"

namespace mold {

class tone_enabled_handler {
 public:
  tone_enabled_handler(options_cache* const database);

  virtual void set_enabled(const bool enabled);
  virtual bool is_enabled();

  virtual ~tone_enabled_handler() = default;

  boost::signals2::signal<void(bool)> signal_changed;

 private:
  options_cache* const m_database;

  bool m_enabled{true};

  logging::logger m_logger;
};
}  // namespace mold

#endif  // MOLD_TONE_ENABLED_HANDLER_HPP
