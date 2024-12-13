#include "std_array_printer.hpp"
#include <algorithm>
#include <iterator>
#include <sstream>

std::string wolf::std_array_printer::to_string(const uint8_t *write,
                                               const std::size_t size) {
  std::stringstream out;
  out << std::hex;
  std::copy_n(write, size, std::ostream_iterator<int>(out, " "));
  return out.str();
}
