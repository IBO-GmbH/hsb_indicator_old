#include "logger.hpp"

#include <boost/log/attributes/constant.hpp>
#include <boost/log/sources/record_ostream.hpp>

using namespace logging;

logger::logger(const std::string &tag) {
  add_attribute("Tag", boost::log::attributes::constant<std::string>(tag));
}
