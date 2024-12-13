#ifndef WOLF_WEBSOCKET_LOGGER_HPP
#define WOLF_WEBSOCKET_LOGGER_HPP

#include "log/logger.hpp"

#include <websocketpp/logger/basic.hpp>

namespace wolf {

template <typename concurrency_type, typename log_level,
          logging::severity severity>
class websocket_logger
    : public websocketpp::log::basic<concurrency_type, log_level> {
 public:
  using base = websocketpp::log::basic<concurrency_type, log_level>;
  websocket_logger(websocketpp::log::level lvl,
                   websocketpp::log::channel_type_hint::value)
      : base(lvl, nullptr), m_logger{"websocketpp"} {}

  void write(websocketpp::log::level channel, std::string const& msg) {
    if (!this->static_test(channel)) {
      return;
    }
    LOGGING_LOG(m_logger, severity)
        << "[" << log_level::channel_name(channel) << "] " << msg;
  }

  void write(websocketpp::log::level channel, char const* msg) {
    const std::string msg_casted(msg);
    this->write(channel, msg_casted);
  }

 private:
  logging::logger m_logger;
};
}

#endif  // WOLF_WEBSOCKET_LOGGER_HPP
