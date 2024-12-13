#ifndef WOLF_ESP3_COMMANDS_HPP
#define WOLF_ESP3_COMMANDS_HPP

#include "log/logger.hpp"
#include "types.hpp"

#include <boost/signals2/signal.hpp>

namespace wolf {

class esp3_sender;
}

namespace wolf {

class esp3_commands {
 public:
  esp3_commands(esp3_sender& sender);

  void get_base_id();

  void handle_response(const types::data& data,
                       const types::data& optional_data);

  boost::signals2::signal<void(const types::id_esp3&)> signal_base_id;

 private:
  logging::logger m_logger;

  esp3_sender& m_sender;

  enum struct waiting_for { nothing, base_id };
  waiting_for m_waiting_for;
};
}

#endif  // WOLF_ESP3_COMMANDS_HPP
