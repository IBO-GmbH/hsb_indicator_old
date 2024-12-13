#ifndef WOLF_ROOM_NAME_INDEX_HANDLER_HPP
#define WOLF_ROOM_NAME_INDEX_HANDLER_HPP

#include <optional>
#include "wolf/room.hpp"

namespace wolf {

class room_name_index_handler {
 public:
  using optional_int = std::optional<int>;
  using rooms = std::vector<room>;

  static optional_int convert_name_to_index(const std::string& name);
  static int get_room_index(const rooms& rooms_);
  static bool is_default_name(const std::string& name);
};
}  // namespace wolf

#endif  // WOLF_ROOM_NAME_INDEX_HANDLER_HPP
