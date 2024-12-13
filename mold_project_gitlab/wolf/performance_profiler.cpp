#include "performance_profiler.hpp"

using namespace wolf;
using namespace logging;

performance_profiler::performance_profiler(logger &logger_,
                                           const severity severity,
                                           const std::string &description)
    : m_logger(logger_),
      m_severity(severity),
      m_description(description),
      m_start(std::chrono::high_resolution_clock::now()) {}

performance_profiler::~performance_profiler() {
  const auto delta = std::chrono::high_resolution_clock::now() - m_start;
  const auto delta_casted =
      std::chrono::duration_cast<std::chrono::milliseconds>(delta);
  LOGGING_LOG(m_logger, m_severity)
      << "performance:" << std::dec << delta_casted.count()
      << "ms, description:" << m_description;
}
