#ifndef LOGGING_SEVERITY_HPP
#define LOGGING_SEVERITY_HPP

#include <istream>
#include <ostream>

namespace logging {

enum class severity { verbose = 3, normal = 2, warning = 1, error = 0 };

std::ostream &operator<<(std::ostream &os, const logging::severity &write);
std::istream &operator>>(std::istream &is, logging::severity &read);
}

#endif  // LOGGING_SEVERITY_HPP
