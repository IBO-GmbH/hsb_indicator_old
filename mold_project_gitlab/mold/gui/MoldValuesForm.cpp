#include "MoldValuesForm.hpp"
#include "ui_MoldValuesForm.h"

#include <boost/optional/optional_io.hpp>
#include "ValueCheckHelper.hpp"
#include "mold/configuration_values.hpp"
#include "mold/program_options_parser.hpp"

using namespace logging;
using namespace mold;

MoldValuesForm::MoldValuesForm(const WolfServices &services,
                               const DataExchange &dataExchange,
                               const wolf::types::uuid_array &room_id,
                               QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MoldValuesForm),
      m_logger("MoldValuesForm"),
      m_services(services),
      m_dataExchange(dataExchange),
      m_room_id(room_id) {
  ui->setupUi(this);

  connect(&m_dataExchange, &DataExchange::configuration_molds_changed, this,
          &MoldValuesForm::updateOutputWithCatch);
  connect(&m_dataExchange, &DataExchange::configuration_values_changed, this,
          &MoldValuesForm::updateOutputWithCatch);
  connect(ui->valveSettings, &QPushButton::clicked, this,
          &MoldValuesForm::openHeatingValveOptions);

  m_state.state = mold::calculation_state::undefined;

  updateOutputWithCatch();
}

MoldValuesForm::~MoldValuesForm() = default;

void MoldValuesForm::updateOutputWithCatch() {
  try {
    updateOutput();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, severity::error)
        << "updateOutput() failed error.what():" << error.what();
  }
}

void MoldValuesForm::updateOutput() {
  boost::optional<wolf::room> room_check = m_dataExchange.get_room(m_room_id);
  if (!room_check) {
    LOGGING_LOG(m_logger, severity::warning)
        << "updateOutput(): could not find room! id:" << m_room_id;
    throw std::runtime_error("MoldValuesForm::updateOutput: room not found");
  }
  const boost::optional<configuration> config_check =
      m_dataExchange.get_configuration_by_room(m_room_id);
  if (!config_check) {
    LOGGING_LOG(m_logger, severity::warning)
        << "updateOutput(): no configuration associated with the room:"
        << m_room;
    throw std::runtime_error(
        "MoldValuesForm::updateOutput: no configuration associated with room");
  }
  m_configuration = config_check.get();
  auto config_id = m_configuration.id;
  if (config_id == wolf::types::uuid_array{}) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to update output because config not found!";
    throw std::runtime_error(
        "MoldValuesForm::updateOutput: failed to update output, config not "
        "found");
  }

  // get values
  const auto values = m_dataExchange.get_configuration_values(config_id);
  if (!values) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to update output because values not found! config_id:"
        << config_id;
    throw std::runtime_error(
        "MoldValuesForm::updateOutput: failed to update output, values not "
        "found");
  }
  if (m_values != values.get()) {
    m_values = values.get();
  }

  // get outdoor value
  m_values_outdoor = m_dataExchange.get_outdoor_value();
  bool humidity_old = false;
  {
    const auto now = std::chrono::system_clock::now();
    humidity_old = updateValuesUI(now);
    updateOutdoorValueUI(now);
  }

  // get state
  const auto state = m_dataExchange.get_configuration_state(config_id);
  if (!state) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to update output because state not found! config_id:"
        << config_id;
    throw std::runtime_error(
        "MoldValuesForm::updateOutput: failed to update output, state not "
        "found");
  }
  bool did_calculation_state_change{false};
  if (m_state != state.get()) {
    did_calculation_state_change = m_state.state != state.get().state;
    m_state = state.get();
  }

  // get mold value
  m_mold_value = m_dataExchange.get_configuration_mold_value(config_id);

  updateStateUI(did_calculation_state_change, humidity_old);
}

const configuration_values &MoldValuesForm::getIndoorValues() const {
  return m_values;
}

const configuration_values_outdoor &MoldValuesForm::getOutdoorValues() const {
  return m_values_outdoor;
}

bool MoldValuesForm::updateValuesUI(const timestamp &now) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateValuesUI(), room:" << m_values;

  if (ValueCheckHelper::isValueValid(m_values.indoor_temperature, now,
                                     m_calculation_timeout) &&
      m_configuration.temperature.is_set()) {
    const float temperature = m_values.indoor_temperature.get().value_;
    ui->indoorTemperature->setText(
        QString::number(static_cast<int>(temperature)));
    const int to_print = std::abs(std::lround(temperature * 10.f) % 10);
    ui->indoorTemperatureDecimal->setText(QString::number(to_print));
    checkColoursTemperatureIndoor(now);
  } else {
    ValueCheckHelper::setNoData(*ui->indoorTemperature,
                                ui->indoorTemperatureDecimal);
  }
  bool humidity_old = false;
  if (ValueCheckHelper::isValueValid(m_values.indoor_humidity, now,
                                     m_calculation_timeout) &&
      m_configuration.humidity.is_set()) {
    ui->indoorHumidity->setText(
        QString::number(std::lround(m_values.indoor_humidity.get().value_)));
    humidity_old = checkColoursHumidityIndoor(now);
  } else {
    ValueCheckHelper::setNoData(*ui->indoorHumidity);
  }
  return humidity_old;
}

void MoldValuesForm::updateOutdoorValueUI(const timestamp &now) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateOutdoorValueUI(), value_outdoor:" << std::dec
      << m_values_outdoor;

  if (ValueCheckHelper::isValueValid(m_values_outdoor.temperature, now,
                                     m_calculation_timeout)) {
    const float temperature = m_values_outdoor.temperature.get().value_;
    ui->outdoorTemperature->setText(
        QString::number(static_cast<int>(temperature)));
    const int to_print = std::abs(std::lround(temperature * 10.f) % 10);
    ui->outdoorTemperatureDecimal->setText(QString::number(to_print));
    checkColoursTemperatureOutdoor(now);
  } else {
    ValueCheckHelper::setNoData(*ui->outdoorTemperature,
                                ui->outdoorTemperatureDecimal);
  }
  if (ValueCheckHelper::isValueValid(m_values_outdoor.humidity, now,
                                     m_calculation_timeout)) {
    const float humidity = m_values_outdoor.humidity.get().value_;
    const int humidity_rounded = std::lround(humidity);
    ui->outdoorHumiditySmall->setText(QString::number(humidity_rounded) + "%");
    checkColoursHumidityOutdoor(now);
  } else {
    ui->outdoorHumiditySmall->setText("");
  }
}

void MoldValuesForm::updateStateUI(const bool did_calculation_state_change,
                                   const bool humidity_old) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateStateUI(), room:" << m_state
      << " did_calculation_state_change:" << std::boolalpha
      << did_calculation_state_change;
  int mold_value_in_percent = static_cast<int>(m_mold_value * 100.f);
  ui->progressBar->setThresholds(
      static_cast<int>(m_services.config->mold_states_.yellow_threshold * 100),
      static_cast<int>(m_services.config->mold_states_.red_threshold * 100));
  ui->progressBar->setValue(mold_value_in_percent);
  if (!did_calculation_state_change) {
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateStateUI(), advanced, changing style (slow!)";
  if (m_state.state == calculation_state::green) {
    if (!humidity_old) ui->humidity->setStyleSheet("color: #66cc00");
  }
  if (calculation_state_is_yellow(m_state.state)) {
    setStyleSheet(
        "#progressWorkaround {"
        "background-color: #cc0000;"
        "}");
    if (!humidity_old) ui->humidity->setStyleSheet("color: #ff9900");
  }
  if (calculation_state_is_red(m_state.state)) {
    if (!humidity_old) ui->humidity->setStyleSheet("color: #cc0000");
  }
}

void MoldValuesForm::checkColoursTemperatureIndoor(
    const MoldValuesForm::timestamp &now) {
  const bool old = ValueCheckHelper::checkIfOld(
      m_values.indoor_temperature.get().timestamp_, now, m_sensor_red_time);
  ValueCheckHelper::changeColourIfOld(
      {ui->indoorTemperature, ui->indoorTemperatureDecimal, ui->unitDegree0},
      old);
}

bool MoldValuesForm::checkColoursHumidityIndoor(
    const MoldValuesForm::timestamp &now) {
  const bool old = ValueCheckHelper::checkIfOld(
      m_values.indoor_humidity.get().timestamp_, now, m_sensor_red_time);
  ValueCheckHelper::changeColourIfOld({ui->indoorHumidity, ui->unitPercent},
                                      old);
  return old;
}

void MoldValuesForm::checkColoursTemperatureOutdoor(
    const MoldValuesForm::timestamp &now) {
  const bool old = ValueCheckHelper::checkIfOld(
      m_values_outdoor.temperature.get().timestamp_, now, m_sensor_red_time);
  ValueCheckHelper::changeColourIfOld(
      {ui->outdoorTemperature, ui->outdoorTemperatureDecimal, ui->unitDegree1},
      old);
}

void MoldValuesForm::checkColoursHumidityOutdoor(
    const MoldValuesForm::timestamp &now) {
  const bool old = ValueCheckHelper::checkIfOld(
      m_values_outdoor.humidity.get().timestamp_, now, m_sensor_red_time);
  ValueCheckHelper::changeColourIfOld({ui->outdoorHumiditySmall}, old);
}
