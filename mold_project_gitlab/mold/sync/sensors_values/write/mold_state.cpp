#include "mold_state.hpp"
#include "mold/sync/sensor_id_mold_state.hpp"

using namespace mold::sync::sensors_values::write;

mold_state::mold_state(handler& writer,
                       mold::configuration_state_handler& state_handler,
                       outdoor_sensor_change_helper& outdoor_helper,
                       room::settings::with_sensors::handler& room_handler)
    : m_logger{"sync::sensors_values::write::mold_state"},
      m_writer(writer),
      m_state_handler(state_handler),
      m_outdoor_helper(outdoor_helper) {
  connect_signals(room_handler);
}

void mold_state::connect_signals(
    room::settings::with_sensors::handler& room_handler) {
  m_signal_connections.emplace_back(
      room_handler.signal_send_fake_values.connect([this](const auto& config) {
        const auto state = m_state_handler.get_state_for_config(config);
        if (state.has_value())
          handle_with_catch(config.id, state.value().state);
      }));
  m_signal_connections.emplace_back(
      m_state_handler.signal_state_changed.connect(
          [this](const auto& id, const auto& state) {
            handle_with_catch(id, state);
          }));
  m_signal_connections.emplace_back(
      m_outdoor_helper.signal_valid_outdoor.connect(
          [this](const auto& id) { handle_with_catch(id, {}); }));
}

void mold_state::handle_with_catch(const wolf::types::uuid_array& id,
                                   const mold::calculation_state& state) {
  try {
    handle(id, state);
  } catch (const std::runtime_error& error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle. error:" << error.what();
  }
}

void mold_state::handle(const wolf::types::uuid_array& id,
                        const mold::calculation_state& state) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle. id:" << id << ", state:" << state;

  const auto sensor_id = sensor_id_mold_state::create(id);
  const auto value = static_cast<float>(state);
  // TODO shoud come from signal
  const auto now = std::chrono::system_clock::now();
  const wolf::sensor_value sensor_value{sensor_id, value, now};
  wolf::sensor_values values;
  values.push_back(sensor_value);
  m_writer.handle(values);
}
