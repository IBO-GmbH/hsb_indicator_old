#ifndef WOLF_COMMAND_HANDLER_HPP
#define WOLF_COMMAND_HANDLER_HPP

#include "log/logger.hpp"
#include "wolf/network/command_parser.hpp"
#include "wolf/network/response/sender.hpp"
#include "wolf/types.hpp"

namespace wolf {

class command_handler {
 public:
  command_handler(network::command_parser& parser);

 private:
  void handle_command_with_catch(network::response::send_helper& response,
                                 const std::string_view script);
  void handle_command(network::response::send_helper& response,
                      const std::string_view script);

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_parsed;
};
}  // namespace wolf

#endif  // WOLF_COMMAND_HANDLER_HPP
