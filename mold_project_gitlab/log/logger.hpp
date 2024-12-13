#ifndef LOGGING_LOGGER_HPP
#define LOGGING_LOGGER_HPP

#include <boost/log/sources/severity_logger.hpp>
#include "log/pragma.hpp"

namespace logging {

class logger : public boost::log::sources::severity_logger<severity> {
 public:
  logger(const std::string &tag);
};
}  // namespace logging

#endif  // LOGGING_LOGGER_HPP
