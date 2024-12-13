#ifndef LOG_JOURNAL_SINK_HPP
#define LOG_JOURNAL_SINK_HPP

#include <boost/log/sinks/basic_sink_backend.hpp>

namespace logging {

class journal_sink : public boost::log::sinks::basic_sink_backend<
                         boost::log::sinks::concurrent_feeding> {
 public:
  void consume(const boost::log::record_view& record);
};

}  // namespace logging

#endif
