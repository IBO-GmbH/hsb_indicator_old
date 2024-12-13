#include "severity.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace logging {

std::ostream &operator<<(std::ostream &os, const severity &write) {
  switch (write) {
    case severity::error:
      os << "error";
      break;
    case severity::normal:
      os << "normal";
      break;
    case severity::verbose:
      os << "verbose";
      break;
    case severity::warning:
      os << "warning";
      break;
    default:
      std::ostringstream error_message;
      error_message << "could not parse log_serverity:"
                    << static_cast<int>(write) << std::flush;
      throw std::invalid_argument(error_message.str());
  }
  return os;
}

std::istream &operator>>(std::istream &is, severity &read) {
  std::string parse;
  is >> parse;
  if (parse == std::string("error")) {
    read = severity::error;
    return is;
  }
  if (parse == std::string("normal")) {
    read = severity::normal;
    return is;
  }
  if (parse == std::string("verbose")) {
    read = severity::verbose;
    return is;
  }
  if (parse == std::string("warning")) {
    read = severity::warning;
    return is;
  }
  throw std::invalid_argument("could not parse log_serverity:" + parse);
}
}
