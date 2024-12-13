#ifndef WOLF_NETWORK_COMMAND_PARSER_HPP
#define WOLF_NETWORK_COMMAND_PARSER_HPP

#include "wolf/network/bond_parser.hpp"
#include "wolf/network/response/send_helper.hpp"
#include "wolf/types.hpp"

namespace wolf::network {

class command_parser {
 public:
  command_parser(bond_parser& parser, response::sender& response);

  boost::signals2::signal<void(response::send_helper&, const std::string_view)>
      signal_parsed;

 private:
  bool handle_with_catch(const Clima::Message& message);
  bool handle(const Clima::Message& message);

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_message;
  response::sender& m_response;
};

}  // namespace wolf::network

#endif
