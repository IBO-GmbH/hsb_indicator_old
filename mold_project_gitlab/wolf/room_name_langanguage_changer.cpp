#include "room_name_langanguage_changer.hpp"

#include "room_name_index_handler.hpp"
#include "room_names.hpp"

using namespace logging;
wolf::room_name_langanguage_changer::room_name_langanguage_changer(
    boost::asio::io_service &service, wolf::room_handler &room_handler)
    : m_logger("room_name_language_changer"),
      m_service(service),
      m_room_handler(room_handler) {}

void wolf::room_name_langanguage_changer::async_set(
    const wolf::languages &language,
    const wolf::room_name_langanguage_changer::completion_callback
        &on_completion) {
  LOGGING_LOG(m_logger, severity::verbose) << "switch language of room names";
  auto rooms_to_update = change_room_names(language);
  m_service.post([this, rooms_to_update, &on_completion] {
    for (const auto &room : rooms_to_update) m_room_handler.update(room);
    on_completion();
  });
}

bool wolf::room_name_langanguage_changer::switch_to_englisch(
    std::string &name) {
  return switch_language(name, room_names_german, room_names_english,
                         default_name_english);
}

bool wolf::room_name_langanguage_changer::switch_to_german(std::string &name) {
  return switch_language(name, room_names_english, room_names_german,
                         default_name_german);
}

bool wolf::room_name_langanguage_changer::switch_language(
    std::string &name,
    const wolf::room_name_langanguage_changer::name_list &from,
    const wolf::room_name_langanguage_changer::name_list &to,
    const std::string &default_name) {
  const auto max_names_from = from.size();
  const auto max_names_to = to.size();
  if (max_names_from != max_names_to) {
    LOGGING_LOG(m_logger, severity::warning)
        << "different amount of rooms between from- and to-language";
    return false;
  }
  for (unsigned int count = 0; count < max_names_from; ++count) {
    if (name == from[count]) {
      name = to[count];
      return true;
    }
    if (room_name_index_handler::is_default_name(name)) {
      name.replace(name.begin(),
                   name.begin() + static_cast<int>(default_name.size()),
                   default_name);
      return true;
    }
  }
  return false;
}

wolf::room_name_langanguage_changer::rooms
wolf::room_name_langanguage_changer::change_room_names(
    const wolf::languages &language) {
  auto rooms_ = m_room_handler.get_all();
  rooms to_update;
  for (auto &room : rooms_) {
    switch (language) {
      case languages::english:
        if (switch_to_englisch(room.name)) to_update.push_back(room);
        continue;
      case languages::german:
        if (switch_to_german(room.name)) to_update.push_back(room);
        continue;
    }
  }
  return to_update;
}
