#include "string_print_helper.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <iterator>
#include <sstream>

std::string string_print_helper::as_hex(const std::string& to_print) {
  std::stringstream out;
  out << std::hex;
  std::copy(to_print.cbegin(), to_print.cend(),
            std::ostream_iterator<int>(out, " "));
  return out.str();
}

std::string string_print_helper::from_at_string(const std::string& to_print) {
  std::string result_print = boost::replace_all_copy(to_print, "\r", "\\r");
  boost::replace_all(result_print, "\n", "\\n");
  return result_print;
}
