#include <boost/stacktrace.hpp>
#include <stdexcept>
#include "log/logger.hpp"

// https://www.boost.org/doc/libs/1_69_0/doc/html/stacktrace/getting_started.html#stacktrace.getting_started.better_asserts

namespace boost {
void assertion_failed_msg(char const* expr, char const* msg,
                          char const* function, char const* /*file*/,
                          long /*line*/) {
  logging::logger logger{"boost::assert"};
  LOGGING_LOG(logger, logging::severity::error)
      << "Expression '" << expr << "' is false in function '" << function
      << "': " << (msg ? msg : "<...>") << ".\n"
      << "Backtrace:\n"
      << boost::stacktrace::stacktrace();
  std::abort();
}

void assertion_failed(char const* expr, char const* function, char const* file,
                      long line) {
  ::boost::assertion_failed_msg(expr, nullptr, function, file, line);
}
}  // namespace boost
