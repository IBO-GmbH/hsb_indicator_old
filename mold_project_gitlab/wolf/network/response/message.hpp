#ifndef WOLF_NETWORK_RESPONSE_MESSAGE_HPP
#define WOLF_NETWORK_RESPONSE_MESSAGE_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2/signal.hpp>
#include <vector>
#include "log/logger.hpp"
#include "mold_protocol_types.h"
#include "wolf/network/bond_parser.hpp"
#include "wolf/types.hpp"

namespace wolf::network::response {

// TODO refactor. SRP. class does two things. scheduling reponses and ensduring
// they don't timeout
class message {
 public:
  using callback = std::function<void(const std::optional<Clima::Message>&)>;

  message(bond_parser& source, boost::asio::io_service& service,
          const std::chrono::steady_clock::duration& response_timeout);

  void register_work_uuid(const types::uuid_array& work_id,
                          const callback& to_call);
  void clear();
  std::size_t get_waiting() const;

  boost::signals2::signal<void()> signal_timeout;

 private:
  bool handle_with_catch(const Clima::Message& message);
  bool handle(const Clima::Message& message);
  void handle_response(const Clima::Response& response_);
  void handle_timeout(const boost::system::error_code& error);

  logging::logger m_logger;
  boost::signals2::scoped_connection m_connection_message;

  using waiting_for_response =
      std::vector<std::pair<types::uuid_array, callback>>;
  waiting_for_response m_waiting_for_response;

  const std::chrono::steady_clock::duration m_response_timeout;
  boost::asio::steady_timer m_timeout;
  bool m_timeout_active{false};

  types::uuid_array m_last_work_id;
};

}  // namespace wolf::network::response
#endif
