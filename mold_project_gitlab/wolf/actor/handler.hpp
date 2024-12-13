#ifndef WOLF_ACTOR_HANDLER_HPP
#define WOLF_ACTOR_HANDLER_HPP

#include <boost/signals2.hpp>
#include "actor.hpp"
#include "log/logger.hpp"
#include "wolf/esp3_sender.hpp"
#include "wolf/sensor_value_handler.hpp"

namespace wolf {

namespace actor {

class handler {
 public:
  using container = std::vector<actor>;

  handler(wolf::esp3_sender* sender, wolf::sensor_value_handler& value_handler);

  void add(const actor& actor);
  void remove(const wolf::types::id_esp3& id);

  void set_base_id(const wolf::types::id_esp3& base_id);
  const wolf::types::id_esp3& get_base_id() const;

  void handle_state_changed(const actor& actor, const bool force);

  container get_all() const;

  boost::signals2::signal<void()> signal_base_id_loaded;

 private:
  void send_rps(const actor& send_to);

  container::iterator find(const types::id_esp3& id);

 private:
  logging::logger m_logger;
  wolf::esp3_sender* const m_sender;
  wolf::sensor_value_handler& m_value_handler;

  container m_container;
  wolf::types::id_esp3 m_base_id{};
};
}  // namespace actor
}  // namespace wolf

#endif  // WOLF_ACTOR_HANDLER_HPP
