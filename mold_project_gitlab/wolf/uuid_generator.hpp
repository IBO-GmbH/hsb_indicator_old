#ifndef WOLF_UUID_GENERATOR_HPP
#define WOLF_UUID_GENERATOR_HPP

#include "types.hpp"

namespace wolf {

class uuid_generator {
 public:
  using uuid_array = types::uuid_array;

  uuid_generator() = delete;

  static uuid_array generate_as_array();
  static std::string generate_as_base64_string();
};
}  // namespace wolf

#endif  // WOLF_UUID_GENERATOR_HPP
