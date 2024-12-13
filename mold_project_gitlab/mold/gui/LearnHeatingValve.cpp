#include "LearnHeatingValve.hpp"
#include "PostToGUIThread.hpp"
#include "mold/heating_valve_helper.hpp"
#include "ui_LearnHeatingValve.h"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/thread_checker.hpp"

#include <QTimer>

mold::LearnHeatingValve::LearnHeatingValve(
    WolfServices &services, const heating_valve_settings &settings,
    QWidget *parent)
    : QWidget(parent),
      ui(new Ui::LearnHeatingValve),
      m_logger{"LearnHeatingValve"},
      m_services(services),
      m_settings(settings) {
  wolf::thread_checker::throw_if_not_ui(
      "LearnValveSettings::LearnValveSettings");

  ui->setupUi(this);
  ui->sensorLearnSuccess->hide();

  connect(ui->back, &QPushButton::clicked, this, &LearnHeatingValve::back);
  connect(ui->removeValves, &QPushButton::clicked, this,
          &LearnHeatingValve::handleRemoveValves);
  m_enocean.signal_parsed.connect(
      [this](const wolf::types::id_esp3 & /*id*/,
             const wolf::enocean_link::parse_results &data) {
        handleEnoceanParsed(data);
      });
  m_connection_enocean_learn = m_services.esp3_parser->signal_learned.connect(
      [this](const wolf::types::id_esp3 &id, const std::uint8_t &org,
             const std::uint8_t &func, const std::uint8_t &type) {
        postToGUIThread([this, id, org, func, type] {
          handleLearned(id, org, func, type);
        });
      });
  m_connection_enocean_data_received =
      m_services.esp3_parser->signal_data_4bs.connect(
          [this](const wolf::types::id_esp3 &id, const wolf::types::data &data,
                 const int /*rssi*/) {
            postToGUIThread([this, id, data] {
              m_enocean.parse_telegram(id, data, false);
            });
          });
}

static bool has_value_a_value(const mold::LearnHeatingValve::value &check) {
  return check.last.is_initialized();
}

struct cast_internal_value_to_sensor_value {
  std::chrono::system_clock::time_point timestamp;

  wolf::sensor_value operator()(const mold::LearnHeatingValve::value &to_cast) {
    wolf::sensor_value result{to_cast.id, to_cast.last.get(), timestamp};
    return result;
  }
};

std::vector<wolf::sensor_value> mold::LearnHeatingValve::getValues() const {
  if (m_last_enocean_id == 0) return {};
  values with_value;
  std::copy_if(m_values.cbegin(), m_values.cend(),
               std::back_inserter(with_value), has_value_a_value);
  std::vector<wolf::sensor_value> result;
  std::transform(with_value.cbegin(), with_value.cend(),
                 std::back_inserter(result),
                 cast_internal_value_to_sensor_value{m_values_timestamp});
  return result;
}

wolf::types::id_esp3 mold::LearnHeatingValve::getLastEnoceanId() const {
  return m_last_enocean_id;
}

std::vector<wolf::sensor_id> mold::LearnHeatingValve::getLearnedIds() const {
  std::vector<wolf::sensor_id> result;
  for (const auto &value : m_values) result.push_back(value.id);
  return result;
}

std::array<uint8_t, 3> mold::LearnHeatingValve::getEep() const {
  return m_last_eep;
}

void mold::LearnHeatingValve::handleRemoveValves() {
  if (m_last_enocean_id != 0) {
    setFormToInitialState();
    m_services.service->post([enocean_handler = m_services.enocean_handler,
                              enocean_id = m_last_enocean_id]() {
      enocean_handler->remove_devices({enocean_id});
    });
    m_last_enocean_id = 0;
    m_values.clear();
  }
  emit removeValves();
}

void mold::LearnHeatingValve::handleLearned(const wolf::types::id_esp3 id,
                                            const uint8_t &org,
                                            const uint8_t &func,
                                            const uint8_t &type) {
  wolf::thread_checker::throw_if_not_ui("LearnHeatingValve::handleLearned");
  auto profiles = m_enocean.register_device(id, org, func, type, false);
  if (profiles.empty()) {
    LOGGING_LOG(m_logger, logging::severity::warning)
        << "sensor profile has no profiles, id:" << id << " org:" << std::hex
        << static_cast<int>(org) << " func:" << static_cast<int>(func)
        << " type:" << static_cast<int>(type);
    return;
  }

  m_last_enocean_id = id;
  m_last_eep = {org, func, type};
  for (const auto &profile : profiles) {
    if (profile.name.empty()) {
      LOGGING_LOG(m_logger, logging::severity::verbose)
          << "skipping invalid heating valve value with empty profile";
      continue;
    }
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "learned profile, type:" << std::hex
        << static_cast<int>(profile.type) << " name:" << profile.name
        << " unit:" << profile.unit << " range from:" << profile.range_from
        << " range to:" << profile.range_to;
    m_values.push_back(createValue(profile.type));
  }
  ui->teachDescription->hide();
  ui->sensorLearnSuccess->show();
  // only connect one valve at a time
  m_connection_enocean_learn.disconnect();

  const auto data_bytes = heating_valve_helper::get_learn_response_bytes();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "send learning answer to valve, data: " << data_bytes
      << ", id: " << m_last_enocean_id;
  sendData(data_bytes);
}

void mold::LearnHeatingValve::handleEnoceanParsed(
    const wolf::enocean_link::parse_results &data) {
  wolf::thread_checker::throw_if_not_ui(
      "LearnHeatingValve::handleEnoceanParsed");
  for (const wolf::enocean_link::parse_result &item : data) {
    if (!isValidValue(item.type)) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "type is not a valid value: 0x" << std::hex << item.type << ", "
          << item.name;
      continue;
    }
    auto found = std::find_if(
        m_values.begin(), m_values.end(),
        [&](const value &value_) { return (value_.type == item.type); });
    if (found == m_values.end()) {
      LOGGING_LOG(m_logger, logging::severity::error)
          << "type not found in values container - must not happen, because "
             "EEP should tell us all available value types";
      throw std::runtime_error(
          "LearnSensorForm::handleEnoceanParsed: type not found in values "
          "container");
    }
    found->last = item.value;
  }
  m_values_timestamp = std::chrono::system_clock::now();
  setSensorValuesToUi();
  const auto data_bytes = heating_valve_helper::get_data_bytes(
      true, m_settings, getCurrentTemperature());
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "send intial data to valve, data: " << data_bytes
      << ", id: " << m_last_enocean_id;
  sendData(data_bytes);
}

bool mold::LearnHeatingValve::isValidValue(const CHANNEL_TYPE &type) {
  return type == S_TEMP || type == S_SETPOINT;
}

void mold::LearnHeatingValve::setSensorValuesToUi() {
  wolf::thread_checker::throw_if_not_ui(
      "LearnHeatingValve::setSensorValuesToUi");
  for (const auto &value_ : m_values) {
    if (!value_.last.has_value()) continue;
    if (!value_.value_label)
      throw std::runtime_error(
          "LearnHeatingValve::setSensorValuesToUi: no label connected to "
          "value");
    value_.value_label->setText(
        QString::number(static_cast<double>(value_.last.get()), 'f', 1));
  }

  // make a success blink
  ui->sensorLearnSuccess->hide();
  QTimer::singleShot(250, ui->sensorLearnSuccess, &QLabel::show);
}

mold::LearnHeatingValve::value mold::LearnHeatingValve::createValue(
    const CHANNEL_TYPE &type) const {
  const auto id =
      wolf::sensor_id_enocean::create(m_last_enocean_id, type, {}, m_last_eep);
  const auto label = getLabelForType(type);
  return {id, type, boost::optional<float>(), label};
}

QLabel *mold::LearnHeatingValve::getLabelForType(
    const CHANNEL_TYPE &type) const {
  if (type == S_TEMP)
    return ui->labelTemperatureValue;
  else if (type == S_SETPOINT)
    return ui->labelValvePositionValue;
  return nullptr;
}

bool mold::LearnHeatingValve::isInvalidType(const unsigned int &type) {
  return type == 0xe017;
}

void mold::LearnHeatingValve::setFormToInitialState() {
  ui->sensorLearnSuccess->hide();
  ui->teachDescription->show();
  ui->labelTemperatureValue->setText("--");
  ui->labelValvePositionValue->setText("--");
}

void mold::LearnHeatingValve::sendData(
    const wolf::types::data &data_bytes) const {
  m_services.service->post([this, data_bytes, sender = m_services.esp3_writer,
                            actor_handler = m_services.actor_handler] {
    const auto base_id = actor_handler->get_base_id();
    sender->send_telegram(data_bytes, RORG_4BS, m_last_enocean_id & base_id,
                          0x00);
  });
}

std::optional<float> mold::LearnHeatingValve::getCurrentTemperature() const {
  std::optional<float> temperature;
  m_services.service->post([this, &temperature,
                            value_handler = m_services.value_handler,
                            room_handler = m_services.rooms] {
    const auto room = room_handler->get(m_settings.room_id);
    temperature = heating_valve_helper::get_current_room_temperature_value(
        room.sensors, *value_handler);
  });
  return temperature;
}

mold::LearnHeatingValve::~LearnHeatingValve() = default;
