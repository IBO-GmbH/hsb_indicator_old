#ifndef LOGGING_PRAGMA_HPP
#define LOGGING_PRAGMA_HPP

#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "log/severity.hpp"

#define LOGGING_LOG(logger, severity_)                               \
  BOOST_LOG_STREAM_WITH_PARAMS(                                      \
      (logger), (::logging::set_get_attrib(                          \
                    "File", ::logging::path_to_filename(__FILE__)))( \
                    ::logging::set_get_attrib("Line", __LINE__))(    \
                    ::boost::log::keywords::severity = (severity_)))
namespace logging {

// Set attribute and return the new value
template <typename value_type>
value_type set_get_attrib(const char *name, value_type value);
extern template int set_get_attrib<int>(const char *name, int);
extern template std::string set_get_attrib<std::string>(const char *name,
                                                        std::string);
// Convert file path to only the filename
std::string path_to_filename(std::string path);
}  // namespace logging

#endif
