#include "pragma.hpp"
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>

std::string logging::path_to_filename(std::string path) {
  return path.substr(path.find_last_of("/\\") + 1);
}

template <typename value_type>
value_type logging::set_get_attrib(const char *name, value_type value) {
  namespace attrs = boost::log::attributes;
  namespace expr = boost::log::expressions;
  auto core = boost::log::core::get();
  auto attr = boost::log::attribute_cast<attrs::mutable_constant<value_type>>(
      core->get_global_attributes()[name]);
  attr.set(value);
  return attr.get();
}

template int logging::set_get_attrib<int>(const char *name, int);
template std::string logging::set_get_attrib<std::string>(const char *name,
                                                          std::string);
