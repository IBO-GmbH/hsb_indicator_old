#ifndef WOLF_UUID_HELPER_HPP
#define WOLF_UUID_HELPER_HPP

#include "wolf/types.hpp"

namespace wolf {

class uuid_helper {
 public:
  uuid_helper() = default;

  types::uuid_array vector_to_array(const types::data& to_cast) const;
  types::data array_to_vector(const types::uuid_array& to_cast) const;
  std::string array_to_string(const types::uuid_array& to_cast) const;
  types::uuid_array string_to_array(const std::string& to_cast) const;
};

}  // namespace wolf
#endif
