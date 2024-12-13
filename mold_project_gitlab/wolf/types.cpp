#include "wolf/types.hpp"
#include <iterator>
#include "wolf/uuid_helper.hpp"

template <class write_type>
std::ostream &format_and_print(std::ostream &os, const write_type &write) {
  os << std::hex;
  std::copy(write.cbegin(), write.cend(), std::ostream_iterator<int>(os, " "));
  return os;
}

std::ostream &std::operator<<(std::ostream &os,
                              const wolf::types::data &write) {
  return format_and_print(os, write);
}

std::ostream &std::operator<<(std::ostream &os,
                              const wolf::types::uuid_array &write) {
  return os << wolf::uuid_helper().array_to_string(write);
}

std::ostream &std::operator<<(std::ostream &os,
                              const wolf::types::id_esp3_array &write) {
  return format_and_print(os, write);
}

std::ostream &std::operator<<(std::ostream &os, const wolf::types::eep &write) {
  return format_and_print(os, write);
}
