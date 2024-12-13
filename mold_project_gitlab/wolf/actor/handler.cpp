#include "handler.hpp"
#include "wolf/actor/sensor_profile.hpp"
#include "wolf/esp3_sender.hpp"
#include "wolf/thread_checker.hpp"

using namespace wolf;
using namespace actor;
using namespace logging;

handler::handler(wolf::esp3_sender *sender, sensor_value_handler &value_handler)
    : m_logger("actor_handler"),
      m_sender{sender},
      m_value_handler(value_handler) {}

void handler::add(const actor &actor) {
  LOGGING_LOG(m_logger, severity::verbose) << "add actor: " << actor;
  const auto found = find(actor.id);
  if (found != m_container.end()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "actor already added, actor:" << actor;
    return;
  }
  m_container.emplace_back(actor);
  m_value_handler.handle_single_value(
      {wolf::actor::sensor_profile::create_sensor_id(actor.id + m_base_id),
       static_cast<float>(actor.state), std::chrono::system_clock::now()});
}

void handler::remove(const wolf::types::id_esp3 &id) {
  const auto found = find(id);
  LOGGING_LOG(m_logger, severity::verbose) << "remove actor with id: " << id;
  if (found == m_container.end()) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "actor already removed, id:" << id;
    return;
  }
  send_rps({id, false});
  m_container.erase(found);
}

void handler::set_base_id(const wolf::types::id_esp3 &base_id) {
  wolf::thread_checker::throw_if_not_service("handler::set_base_id");
  LOGGING_LOG(m_logger, severity::verbose)
      << "setting base_id:" << std::hex << base_id;
  m_base_id = base_id;
  signal_base_id_loaded();
}

const wolf::types::id_esp3 &handler::get_base_id() const { return m_base_id; }

void handler::send_rps(const actor &send_to) {
  if (m_sender == nullptr) {
    return;
  }
  const wolf::types::id_esp3 base_id = m_base_id;
  const wolf::types::id_esp3 actor_id = send_to.id;
  if (send_to.state) {
    m_sender->send_rps(0x10, base_id + actor_id, 0x30);
    m_sender->send_rps(0x00, base_id + actor_id, 0x20);
  } else {
    m_sender->send_rps(0x30, base_id + actor_id, 0x30);
    m_sender->send_rps(0x00, base_id + actor_id, 0x20);
  }
}

void handler::handle_state_changed(const actor &actor, const bool force) {
  LOGGING_LOG(m_logger, severity::normal) << "handle state of actor: " << actor;
  const auto found = find(actor.id);
  if (found == m_container.end()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "handle_state_changed found == m_container.end(), id:" << actor.id;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << std::hex << (m_base_id + found->id);
  if (found->state == actor.state && !force) return;
  found->state = actor.state;
  send_rps(*found);
  m_value_handler.handle_single_value(
      {wolf::actor::sensor_profile::create_sensor_id(actor.id + m_base_id),
       static_cast<float>(actor.state), std::chrono::system_clock::now()});
}

handler::container handler::get_all() const { return m_container; }

handler::container::iterator handler::find(const wolf::types::id_esp3 &id) {
  return std::find_if(
      m_container.begin(), m_container.end(),
      [&id](const container::value_type &item) { return item.id == id; });
}
