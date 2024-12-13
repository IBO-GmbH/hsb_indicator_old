#include "substrate_type.hpp"
#include "mold/sync/sensor_id_substrate.hpp"

mold::sync::sensors_values::write::substrate_type::substrate_type(
    mold::sync::sensors_values::write::handler &writer,
    mold::options_handler &options, mold::configuration_handler &configurations,
    room::settings::with_sensors::handler &room_handler)
    : m_logger{"sync::sensors_values::write::substrate_type"},
      m_writer(writer),
      m_options(options),
      m_configurations(configurations) {
  connect_signals(room_handler);
}

void mold::sync::sensors_values::write::substrate_type::connect_signals(
    room::settings::with_sensors::handler &room_handler) {
  m_signal_connections.emplace_back(
      m_options.signal_substrate_type_changed.connect(
          [this]() { handle_box_with_catch(); }));
  const auto configuration_callback =
      [this](const configuration &configuration_) {
        handle_configuration_with_catch(configuration_);
      };
  m_signal_connections.emplace_back(
      room_handler.signal_send_fake_values.connect(configuration_callback));
}

void mold::sync::sensors_values::write::substrate_type::
    handle_box_with_catch() {
  try {
    handle_box();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle_box. error:" << error.what();
  }
}

void mold::sync::sensors_values::write::substrate_type::handle_box() {
  const auto type = m_options.get_substrate_type();
  const auto configurations = m_configurations.get_all();
  for (const auto &configuration : configurations) {
    if (configuration.mold_settings_ &&
        configuration.mold_settings_->substrate_type_ !=
            mold::substrate_type::none)
      continue;
    send(configuration.id, type);
  }
}

void mold::sync::sensors_values::write::substrate_type::
    handle_configuration_with_catch(const mold::configuration &configuration_) {
  try {
    handle_configuration(configuration_);
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to handle_configuration. error:" << error.what();
  }
}

void mold::sync::sensors_values::write::substrate_type::handle_configuration(
    const mold::configuration &configuration_) {
  if (!configuration_.mold_settings_ ||
      configuration_.mold_settings_->substrate_type_ ==
          mold::substrate_type::none) {
    const auto type = m_options.get_substrate_type();
    send(configuration_.id, type);
    return;
  }
  const auto type = configuration_.mold_settings_->substrate_type_;
  send(configuration_.id, type);
}

void mold::sync::sensors_values::write::substrate_type::send(
    const wolf::types::uuid_array &config_id,
    const mold::substrate_type &type) {
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "handle. id:" << config_id << ", type:" << type;

  const auto sensor_id = sensor_id_substrate::create(config_id);
  const auto type_value = static_cast<float>(type);
  const auto now = std::chrono::system_clock::now();
  const wolf::sensor_value sensor_value{sensor_id, type_value, now};
  wolf::sensor_values values;
  values.push_back(sensor_value);
  m_writer.handle(values);
}
