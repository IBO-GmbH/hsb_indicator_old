#ifndef MOLD_SYNC_BOX_SETTINGS_READ_PARSER_HPP
#define MOLD_SYNC_BOX_SETTINGS_READ_PARSER_HPP

#include "mold/sync/box_settings/entity.hpp"
#include "wolf/network/bond_parser.hpp"
#include "wolf/network/response/send_helper.hpp"

namespace mold::sync::box_settings::read {
class parser {
 public:
  parser(wolf::network::bond_parser& bond_parser_,
         wolf::network::response::sender& response);

  boost::signals2::signal<void(wolf::network::response::send_helper&,
                               const entity&)>
      signal_parsed;

 private:
  bool handle_with_catch(const Clima::Message& message);
  bool handle(const Clima::Message& message);

  logging::logger m_logger;
  std::vector<boost::signals2::scoped_connection> m_signal_connections;
  wolf::network::response::sender& m_response;
};
}  // namespace mold::sync::box_settings::read

#endif
