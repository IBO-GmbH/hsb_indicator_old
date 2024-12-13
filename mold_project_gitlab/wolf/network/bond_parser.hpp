#ifndef WOLF_NETWORK_BOND_PARSER_HPP
#define WOLF_NETWORK_BOND_PARSER_HPP

#include <boost/signals2/signal.hpp>
#include "log/logger.hpp"
#include "mold_protocol_reflection.h"
#include "wolf/websocket_sync.hpp"

namespace wolf::network {

class bond_parser {
 public:
  bond_parser(wolf::websocket_sync& source);

  struct any_true {
    using result_type = bool;
    template <typename InputIterator>
    bool operator()(InputIterator first, InputIterator last) const {
      return std::any_of(first, last, [](const bool& check) { return check; });
    }
  };
  boost::signals2::signal<bool(const Clima::Message&), any_true> signal_message;

 private:
  void handle_message_with_catch(const std::string& to_parse);
  void handle_message(const std::string& to_parse);

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_message;
};
}  // namespace wolf::network

#endif
