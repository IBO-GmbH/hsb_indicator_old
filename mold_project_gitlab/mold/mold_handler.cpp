#include "mold_handler.hpp"
#include "mold/sensor_id_evaluation_humidity.hpp"
#include "mold/sensor_id_sprout.hpp"

using namespace mold;

mold_handler::mold_handler(
    configuration_handler &handler, configuration_values_handler &value,
    configuration_state_handler &state,
    evaluation_humidity::handler &evaluation_humidity, sprout::handler &sprout,
    wolf::sensor_value_handler &value_handler, wolf::room_handler &room_handler,
    mold_value_handler &mold_value_, options_handler &options)
    : m_logger{"mold_handler"},
      m_handler(handler),
      m_value(value),
      m_state(state),
      m_evaluation_humidity(evaluation_humidity),
      m_sprout(sprout),
      m_value_handler(value_handler),
      m_rooms(room_handler),
      m_mold_value(mold_value_),
      m_options(options) {
  setup_value_handler();
  setup_handler();
  setup_configuration_calculator();
  setup_mold_value();
  setup_configuration_values();
  setup_options_handler();
}

void mold_handler::setup_value_handler() {
  m_value_handler.signal_values.connect(
      [this](const auto &values) { handle_values(values); });
}

void mold_handler::setup_handler() {
  m_handler.signal_outdoor_changed.connect(
      [this](const wolf::outdoor_sensor &old,
             const wolf::outdoor_sensor &new_) {
        handle_handler_outdoor_changed(old, new_);
      });
  m_handler.signal_added.connect([this](const mold::configuration &added) {
    handle_configuration_added(added);
  });
  m_handler.signal_updated.connect([this](const mold::configuration &updated) {
    handle_configuration_updated(updated);
  });
  m_handler.signal_removed.connect([this](const wolf::types::uuid_array &id) {
    handle_configuration_removed(id);
  });
}

void mold_handler::setup_configuration_calculator() {
  m_evaluation_humidity.signal_median.connect(
      [this](const wolf::types::uuid_array &config_id,
             const wolf::sensor_value humidity) {
        handle_evaluation_humidity_median(config_id, humidity);
      });
  m_evaluation_humidity.signal_filtered.connect(
      [this](const wolf::sensor_value humidity) {
        handle_evaluation_humidity_filtered(humidity);
      });
}

void mold_handler::setup_mold_value() {
  m_mold_value.signal_value.connect(
      [this](const auto &value) { handle_mold_value(value); });
}

void mold_handler::setup_configuration_values() {
  m_value.signal_value.connect(
      [this](const auto &value) { handle_configuration_values(value); });
  m_value.signal_values_outdoor.connect([this](const auto &value) {
    handle_configuration_outdoor_values(value);
  });
}

void mold_handler::setup_options_handler() {
  m_options.signal_substrate_type_changed.connect(
      [this] { handle_substrate_changed_all(); });
}

void mold_handler::handle_configuration_added(const configuration &added) {
  try {
    m_state.add(added);
    m_sprout.add_configuration(added.id);
    m_value.add(added);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_configuration_added, what():" << error.what();
  }
}

void mold_handler::handle_configuration_updated(const configuration &updated) {
  try {
    m_state.update(updated);
    m_value.update(updated);
    handle_substrate_changed_one(updated);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_configuration_updated, what():" << error.what();
  }
}

void mold_handler::handle_configuration_removed(
    const wolf::types::uuid_array &id) {
  try {
    m_state.remove(id);
    m_value.remove(id);
    m_sprout.remove_configuration(id);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_configuration_removed, what():" << error.what();
  }
}

void mold_handler::handle_evaluation_humidity_median(
    const wolf::types::uuid_array &config_id,
    const wolf::sensor_value &humidity) {
  try {
    // may get called two times for the same room. if a sensor is outdoor AND
    // indoor.
    const auto config = m_handler.get(config_id);
    auto type = m_options.get_substrate_type();
    if (config.has_value() && config->mold_settings_.has_value()) {
      const auto config_type = config->mold_settings_->substrate_type_;
      if (config_type != substrate_type::none) type = config_type;
    }
    m_sprout.handle_evaluation_humidity(config_id, humidity, type);
    m_mold_value.recalculate(config_id, humidity.timestamp);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_evaluation_humidity_median, what():" << error.what();
  }
}

void mold_handler::handle_evaluation_humidity_filtered(
    const wolf::sensor_value &humidity) {
  try {
    // may get called two times for the same room. if a sensor is outdoor AND
    // indoor.
    m_value_handler.handle_single_value(humidity);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_evaluation_humidity_filtered, what():"
        << error.what();
  }
}

void mold_handler::handle_mold_value(const mold_value &value) {
  try {
    auto id = mold::sensor_id_sprout::create(value.configuration);
    auto value_in_percent = value.percentage * 100.f;
    m_value_handler.handle_single_value({id, value_in_percent, value.timestamp});
    m_state.handle_mold_value(value);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_mold_value, what():" << error.what();
  }
}

void mold_handler::handle_values(const wolf::sensor_values &values) {
  try {
    m_value.handle_values(values);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_values, what():" << error.what();
  }
}

void mold_handler::handle_handler_outdoor_changed(
    const wolf::outdoor_sensor &old, const wolf::outdoor_sensor &new_) {
  try {
    m_value.set_outdoor_sensor(new_);
    if (old.is_set()) {
      std::vector<wolf::sensor_id> old_outdoor_ids;
      if (old.temperature.is_set()) old_outdoor_ids.push_back(old.temperature);
      if (old.humidity.is_set()) old_outdoor_ids.push_back(old.humidity);
      m_rooms.remove_unused_sensors(old_outdoor_ids);
    }
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_handler_outdoor_changed, what():" << error.what();
  }
}

void mold_handler::handle_configuration_values(
    const configuration_values &values) {
  try {
    m_evaluation_humidity.handle_indoor(values);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_configuration_values, what():" << error.what();
  }
}

void mold_handler::handle_configuration_outdoor_values(
    const configuration_values_outdoor &outdoor) {
  try {
    m_evaluation_humidity.handle_outdoor(outdoor);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_configuration_outdoor_values, what():"
        << error.what();
  }
}

void mold_handler::handle_substrate_changed_one(const configuration &config) {
  try {
    const auto eval_hum_id = sensor_id_evaluation_humidity::create(config.id);
    const auto eval_hum_value = m_value_handler.get_value(eval_hum_id);
    if (!eval_hum_value.has_value()) return;
    auto type = m_options.get_substrate_type();
    if (config.mold_settings_.has_value()) {
      const auto config_type = config.mold_settings_->substrate_type_;
      if (config_type != substrate_type::none) type = config_type;
    }
    m_sprout.handle_evaluation_humidity(config.id, eval_hum_value.get(), type);
  } catch (const std::exception &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed handle_substrate_change_one, what():" << error.what();
  }
}

void mold_handler::handle_substrate_changed_all() {
  const auto configs = m_handler.get_all();
  for (const auto &config : configs) handle_substrate_changed_one(config);
}
