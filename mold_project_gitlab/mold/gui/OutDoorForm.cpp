#include "OutDoorForm.hpp"
#include <QTimer>
#include "WolfServices.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/PostToGUIThread.hpp"
#include "ui_OutDoorForm.h"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

OutDoorForm::OutDoorForm(WolfServices& services, DataExchange& dataExchange,
                         QWidget* parent)
    : QWidget(parent),
      ui(new Ui::OutDoorForm),
      m_logger{"OutDoorForm"},
      m_services(services),
      m_dataExchange(dataExchange) {
  ui->setupUi(this);
  ui->sensorLearnSuccess->hide();
  ui->foreignSensors->hide();

  connect(ui->back, &QPushButton::clicked, this, &OutDoorForm::handleBack);
  connect(ui->changeOutdoorSensorOffset, &QPushButton::clicked, this,
          &OutDoorForm::sensorOffset);

  m_enocean.clear();
  m_last_data.clear();
  m_connection_learn = connect(&m_dataExchange, &DataExchange::learned, this,
                               &OutDoorForm::handleLearned);
  connect(&m_dataExchange, &DataExchange::data_4bs_or_vld, this,
          [this](const wolf::types::id_esp3& id, const wolf::types::data& data,
                 const int /*rssi*/) {
            wolf::thread_checker::throw_if_not_ui("OutDoorForm::OutDoorForm");
            if (id == m_last_enocean_id) m_last_data = data;
            m_enocean.parse_telegram(id, data, true);
          });

  m_connection_enocean_data = m_enocean.signal_parsed.connect(
      [this](const wolf::types::id_esp3& /*id*/,
             const wolf::enocean_link::parse_results& data) {
        handleEnoceanParsed(data);
      });

  connect(&m_dataExchange, &DataExchange::foreign_sensors, this,
          &OutDoorForm::handleForeignSensors);
  connect(ui->foreignSensors, &QPushButton::pressed, this,
          &OutDoorForm::chooseForeignSensor);

  // demand foreign sensor list
  m_dataExchange.demand_foreign_sensors();

  handleDescription(false);
}

OutDoorForm::~OutDoorForm() = default;

void OutDoorForm::setSensorValuesToUi() {
  wolf::thread_checker::throw_if_not_ui("OutDoorForm::setSensorValuesToUi");
  if (m_temperature)
    ui->outdoorTemperature->setText(
        QString::number(static_cast<double>(m_temperature.get()), 'g', 2));
  if (m_humidity)
    ui->outdoorHumidity->setText(
        QString::number(static_cast<int>(m_humidity.get())));
  LOGGING_LOG(m_logger, severity::verbose)
      << "m_first_time_values_to_ui:" << std::boolalpha
      << m_first_time_values_to_ui;
  if (m_first_time_values_to_ui) {
    ui->teachDescription->hide();
    ui->changeOutdoorSensorOffset->hide();
    ui->foreignSensors->hide();
    ui->sensorLearnSuccess->show();
    m_first_time_values_to_ui = false;
    resizeAndRearrange(true);
    return;
  }
  // do a success blink
  ui->sensorLearnSuccess->hide();
  QTimer::singleShot(250, ui->sensorLearnSuccess, &QLabel::show);
}

void OutDoorForm::handleLearned(const wolf::types::id_esp3& id,
                                const uint8_t& org, const uint8_t& func,
                                const uint8_t& type) {
  wolf::thread_checker::throw_if_not_ui("OutDoorForm::handleLearned");
  // ensure has S_TEMP
  auto profiles = m_enocean.register_device(id, org, func, type, true);
  bool has_S_TEMP{false};
  for (const auto& profile : profiles)
    if (profile.type == S_TEMP) {
      has_S_TEMP = true;
      break;
    }
  if (!has_S_TEMP) {
    LOGGING_LOG(m_logger, severity::warning)
        << "sensor profile has no S_TEMP, id:" << id << " org:" << std::hex
        << static_cast<int>(org) << " func:" << static_cast<int>(func)
        << " type:" << static_cast<int>(type);
    return;
  }
  for (const auto& profile : profiles) {
    if (profile.type == S_TEMP)
      m_outdoor_id.temperature = wolf::sensor_id_enocean::create(
          id, profile.type, profile.subchannel, {org, func, type});
    if (profile.type == S_RELHUM)
      m_outdoor_id.humidity = wolf::sensor_id_enocean::create(
          id, profile.type, profile.subchannel, {org, func, type});
  }

  m_last_enocean_id = id;
  m_last_eep = {org, func, type};

  disconnect(m_connection_learn);
}

void OutDoorForm::handleForeignSensors() {
  const auto& sensor_list = m_dataExchange.get_foreign_sensors();

  LOGGING_LOG(m_logger, severity::verbose)
      << "got foreign sensors, count:" << sensor_list.size();
  if (sensor_list.empty()) return;

  ui->foreignSensors->show();
  ui->teachDescription->setText(
      tr("Please press the LEARN button or press or choose a foreign sensor."));
  ui->teachDescription->setStyleSheet("QLabel {font-size: 12pt;}");
  handleDescription(true);
}

void OutDoorForm::handleEnoceanParsed(
    const wolf::enocean_link::parse_results& data) {
  if (m_last_enocean_id == 0) return;  // nothing learned yet
  for (const wolf::enocean_link::parse_result& item : data) {
    if (item.type == S_TEMP) m_temperature = item.value;
    if (item.type == S_RELHUM) m_humidity = item.value;
  }
  m_timepoint_values = std::chrono::system_clock::now();
  setSensorValuesToUi();
}

void OutDoorForm::handleBack() {
  setEnabled(false);
  if (m_last_enocean_id != 0) {
    m_services.service->post([this]() {
      try {
        saveToServices();
      } catch (const std::runtime_error& error) {
        LOGGING_LOG(m_logger, logging::severity::error)
            << "could not save out door sensor, error:" << error.what();
      }
      postToGUIThread([this] { back(); });
    });
  } else
    emit back();
}

void OutDoorForm::saveToServices() {
  wolf::thread_checker::throw_if_not_service("OutDoorForm::saveToServices");
  const auto registered_profiles = m_services.enocean_handler->register_(
      m_last_enocean_id, m_last_eep, true);
  for (const auto& profile : registered_profiles)
    m_services.learn_handler->learn(profile);
  m_services.outdoor_change_helper->set(m_outdoor_id);
  wolf::sensor_values sensor_values;
  if (m_temperature) {
    const wolf::sensor_value value{m_outdoor_id.temperature,
                                   m_temperature.get(), m_timepoint_values};
    sensor_values.push_back(value);
  }
  if (m_humidity) {
    const wolf::sensor_value value{m_outdoor_id.humidity, m_humidity.get(),
                                   m_timepoint_values};
    sensor_values.push_back(value);
  } else
    m_services.configuration_values->reset_outdoor_humidity_value();
  m_services.value_handler->handle_list(sensor_values);
}

void OutDoorForm::expandDescription(const bool temperature, const bool humidity,
                                    const bool foreign,
                                    const bool foreign_list) {
  const auto both = temperature && humidity;
  auto description = ui->teachDescription->text();
  description += tr(" Current sensor has ");
  if (temperature) description += tr("temperature");
  if (both) description += tr(" and ");
  if (humidity) description += tr("humidity");
  if (foreign) description += tr(" (foreign)");
  if (description != ui->teachDescription->text()) {
    if (foreign_list || (foreign && !foreign_list))
      ui->teachDescription->setStyleSheet("QLabel {font-size: 11pt;}");
    else
      ui->teachDescription->setStyleSheet("QLabel {font-size: 12pt;}");
  }
  description += ".";
  ui->teachDescription->setText(description);
}

void OutDoorForm::handleDescription(const bool foreign_list) {
  const auto hideSensorOffset = [this] {
    ui->changeOutdoorSensorOffset->hide();
    ui->teachDescription->resize(320, ui->teachDescription->height());
  };

  const auto outdoor_sensor = m_dataExchange.get_outdoor_sensor_id();
  if (!outdoor_sensor.is_set()) {
    hideSensorOffset();
  } else {
    const auto foreign = outdoor_sensor.temperature.has_owner() ||
                         outdoor_sensor.humidity.has_owner();
    expandDescription(outdoor_sensor.temperature.is_set(),
                      outdoor_sensor.humidity.is_set(), foreign, foreign_list);
    if (foreign) hideSensorOffset();
  }
  resizeAndRearrange(false);
}

void OutDoorForm::resizeAndRearrange(const bool show_values) {
  ui->teachDescription->adjustSize();
  ui->teachDescription->move(10, 200 - ui->teachDescription->height());
  auto label_y = ui->teachDescription->y();
  if (show_values) label_y = ui->sensorLearnSuccess->y();
  // number widgets are bigger than they have to be to display the bars, so to
  // use this space we apply an offset (20)
  auto space =
      (label_y - ui->settingsHead->height() - ui->sensors->height() + 20) / 2;
  space = std::max(space, 5);
  ui->sensors->move(10, ui->settingsHead->height() + space);
}
