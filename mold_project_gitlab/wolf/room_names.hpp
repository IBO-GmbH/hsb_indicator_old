#ifndef WOLF_ROOM_NAMES_HPP
#define WOLF_ROOM_NAMES_HPP

#include <string>
#include <vector>

namespace wolf {
static std::vector<std::string> room_names_german = {
    "Wohnzimmer", "Kinderzimmer", "Schlafzimmer", "Küche",
    "Keller",     "Büro",         "Badezimmer"};
static std::vector<std::string> room_names_english = {
    "Living room", "Nursery", "Bedroom", "Kitchen",
    "Cellar",      "Office",  "Bathroom"};
static std::vector<std::string> default_room_names{"Raum ", "Room "};
static std::string default_name_german = default_room_names[0];
static std::string default_name_english = default_room_names[1];
}  // namespace wolf
#endif  // WOLF_ROOM_NAMES_HPP
