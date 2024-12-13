#include "room_name_index_handler.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "wolf/room_names.hpp"

wolf::room_name_index_handler::optional_int
wolf::room_name_index_handler::convert_name_to_index(const std::string &name) {
  try {
    for (const auto &default_name : default_room_names)
      if (boost::algorithm::starts_with(name, default_name)) {
        std::string index_string = name.substr(default_name.length());
        const int index = boost::lexical_cast<int>(index_string);
        return index;
      }
    return optional_int();
  } catch (const boost::bad_lexical_cast &) {
    return optional_int();
  }
}

int wolf::room_name_index_handler::get_room_index(
    const wolf::room_name_index_handler::rooms &rooms_) {
  if (rooms_.size() == 0) return 1;
  int room_index = 0;
  for (auto &room : rooms_) {
    auto next_index = convert_name_to_index(room.name);
    if (!next_index.has_value()) continue;
    room_index = std::max(room_index, next_index.value());
  }
  return room_index + 1;
}

bool wolf::room_name_index_handler::is_default_name(const std::string &name) {
  const auto index = convert_name_to_index(name);
  return index.has_value();
}
