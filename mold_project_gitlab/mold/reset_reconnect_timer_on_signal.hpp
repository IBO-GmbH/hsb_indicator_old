#ifndef MOLD_RESET_RECONNECT_TIMER_ON_SIGNAL_HPP
#define MOLD_RESET_RECONNECT_TIMER_ON_SIGNAL_HPP

#include <boost/asio/signal_set.hpp>
#include "mold/reconnect_handler.hpp"

namespace mold {

class reset_reconnect_timer_on_signal {
 public:
  reset_reconnect_timer_on_signal(boost::asio::io_context& context,
                                  reconnect_handler& reconnector);

 private:
  void async_wait();
  void handle(const boost::system::error_code& error);

  logging::logger m_logger;
  boost::asio::signal_set m_signals;
  reconnect_handler& m_reconnector;
};
}  // namespace mold

#endif
