#ifndef MOLD_SYNC_ROOM_SETTINGS_PARSER_HPP
#define MOLD_SYNC_ROOM_SETTINGS_PARSER_HPP

#include "mold/building_type.hpp"
#include "mold/substrate_type.hpp"
#include "wolf/network/bond_parser.hpp"
#include "wolf/network/response/send_helper.hpp"
#include "wolf/types.hpp"

namespace mold::sync::room::settings {

class parser {
 public:
  parser(wolf::network::bond_parser& bond_parser_,
         wolf::network::response::sender& response);

  struct information {
    wolf::types::uuid_array room;
    std::string name;
    building_type building_type_;
    float threshold_factor;
    substrate_type substrate_type_;
  };
  boost::signals2::signal<void(wolf::network::response::send_helper&,
                               const information&)>
      signal_parsed;

 private:
  bool handle_with_catch(const Clima::Message& message);
  bool handle(const Clima::Message& message);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  wolf::network::response::sender& m_response;
};

std::ostream& operator<<(std::ostream& out,
                         const parser::information& to_print);

}  // namespace mold::sync::room::settings

#endif
