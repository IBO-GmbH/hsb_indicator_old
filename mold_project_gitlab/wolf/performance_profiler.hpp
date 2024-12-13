#ifndef WOLF_PERFORMANCE_PROFILER_HPP
#define WOLF_PERFORMANCE_PROFILER_HPP

#include <chrono>
#include "log/logger.hpp"

namespace wolf {

class performance_profiler {
 public:
  performance_profiler(logging::logger &logger_,
                       const logging::severity severity,
                       const std::string &description);
  ~performance_profiler();

 private:
  logging::logger &m_logger;
  const logging::severity m_severity;
  const std::string m_description;

  std::chrono::high_resolution_clock::time_point m_start;
};
}

#endif  // WOLF_PERFORMANCE_PROFILER_HPP
