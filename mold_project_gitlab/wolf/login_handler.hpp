#ifndef WOLF_LOGIN_HANDLER_HPP
#define WOLF_LOGIN_HANDLER_HPP

#include <boost/signals2/signal.hpp>
#include "network/login/sender.hpp"
#include "wolf/key_value_cache.hpp"

namespace wolf {

class login_handler {
 public:
  struct device_information {
    std::string version;
    types::id id;
    types::data secret;
  };
  virtual ~login_handler() = default;
  virtual bool is_logged_in() const = 0;

  boost::signals2::signal<void(bool)> signal_logged_in;
  boost::signals2::signal<void()> signal_login_error;
  boost::signals2::signal<void()> signal_resync;
};

class login_handler_implementation : public login_handler {
 public:
  explicit login_handler_implementation(network::login::sender& network_login,
                                        key_value_cache& options,
                                        const device_information& information);
  void handle_network_connected();
  void handle_network_closed();

  bool is_logged_in() const override;

 protected:
  void handle_logged_in();

 private:
  void handle_response(
      const std::optional<network::login::response::data>& response);
  void read_or_create_database_id();

  logging::logger m_logger;
  network::login::sender& m_network_login;
  key_value_cache& m_options;

  bool m_logged_in;
  const device_information m_device_information;
  types::uuid_array m_database_version;
};

}  // namespace wolf

#endif  // WOLF_LOGIN_HANDLER_HPP
