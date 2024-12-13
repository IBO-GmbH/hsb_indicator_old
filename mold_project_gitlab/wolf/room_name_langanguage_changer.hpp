#ifndef WOLF_ROOM_NAME_LANGANGUAGE_CHANGER_HPP
#define WOLF_ROOM_NAME_LANGANGUAGE_CHANGER_HPP

#include <boost/asio/io_service.hpp>
#include "languages.hpp"
#include "room_handler.hpp"

namespace wolf {

class room_name_langanguage_changer {
 public:
  room_name_langanguage_changer(boost::asio::io_service& service,
                                room_handler& room_handler);
  using completion_callback = std::function<void()>;
  void async_set(const languages& language,
                 const completion_callback& on_completion);

 private:
  bool switch_to_englisch(std::string& name);
  bool switch_to_german(std::string& name);
  using name_list = std::vector<std::string>;
  bool switch_language(std::string& name, const name_list& from,
                       const name_list& to, const std::string& default_name);
  using rooms = std::vector<room>;
  rooms change_room_names(const languages& language);

 private:
  logging::logger m_logger;
  boost::asio::io_service& m_service;
  room_handler& m_room_handler;
};
}  // namespace wolf

#endif  // WOLF_ROOM_NAME_LANGANGUAGE_CHANGER_HPP
