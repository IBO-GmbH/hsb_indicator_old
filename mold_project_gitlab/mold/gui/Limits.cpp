#include "Limits.hpp"
#include "ui_Limits.h"

#include "ButtonDisableHandler.hpp"
#include "ControllerLabelHandler.hpp"
#include "PostToGUIThread.hpp"
#include "mold/gui/DataExchange.hpp"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

Limits::Limits(WolfServices &services, DataExchange &dataExchange,
               const LimitTypes &limit_type, const ControllerTypes &type,
               const wolf::types::uuid_array &room_id, QWidget *parent)
    : QWidget(parent),
      m_logger("Limits"),
      ui(new Ui::Limits),
      m_services(services),
      m_dataExchange(dataExchange),
      m_type(limit_type),
      m_room_id(room_id) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &Limits::handleBack);

  connect(ui->lower1, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleLower(ui->value1, {ui->lower1}, {ui->raise1});
  });
  connect(ui->raise1, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleRaise(ui->value1, {ui->lower1}, {ui->raise1});
  });

  connect(ui->lower2, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleLower(ui->value2, {ui->lower2}, {ui->raise2});
  });
  connect(ui->raise2, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleRaise(ui->value2, {ui->lower2}, {ui->raise2});
  });

  connect(ui->previous, &QPushButton::clicked, this, &Limits::handlePrevious);
  connect(ui->next, &QPushButton::clicked, this, &Limits::handleNext);

  ControllerLabelHandler::changeLabelLimits(type, ui->settingLabel);
  changeLimitType();
  loadLimits();
  saveOldLimits();
}

Limits::~Limits() = default;

void Limits::handleBack() {
  setDisabled(true);
  setLimits([this] { emit back(); });
}

void Limits::handlePrevious() {
  setDisabled(true);
  setLimits([this] { emit previous(); });
}

void Limits::handleNext() {
  setDisabled(true);
  setLimits([this] { emit next(); });
}

void Limits::changeToHumidity() {
  setLabelsHumidity();
  setValuesMax();
  setValuesDRH();
}

void Limits::changeToTemperature() {
  setLabelsTemperature();
  setValuesMin();
  setValuesReference();
}

void Limits::changeToHumidityBorder() {
  setLabelsHumidityBorder();
  setValuesHumidityBorder();
  setValuesDRH();
}

void Limits::changeToSproutValue() {
  setLabelsSproutValue();
  setValuesSproutValue();
  setValuesSproutValueDifference();
}

void Limits::changeToVOC() {
  setLabelsVOC();
  setValuesVOC();
  setValuesVOCDifference();
}

void Limits::changeToOutdoorTemperature() {
  setOneInvisible();
  setLabelsOutdoorTemperature();
  setValuesOutdoorTemperature();
}

void Limits::changeLimitType() {
  switch (m_type) {
    case LimitTypes::humidity:
      changeToHumidity();
      break;
    case LimitTypes::humidityBorder:
      changeToHumidityBorder();
      break;
    case LimitTypes::temperature:
      changeToTemperature();
      break;
    case LimitTypes::CO2:
      break;
    case LimitTypes::sproutValue:
      changeToSproutValue();
      break;
    case LimitTypes::VOC:
      changeToVOC();
      break;
    case LimitTypes::outdoorTemperature:
      changeToOutdoorTemperature();
      break;
  }
}

void Limits::setOneInvisible() {
  ui->label2->setVisible(false);
  ui->value2->setVisible(false);
  ui->lower2->setVisible(false);
  ui->raise2->setVisible(false);
}

void Limits::setLabelsHumidity() {
  ui->unit->setText(tr("Humidity"));
  ui->label1->setText(tr("maximum\nin %"));
  ui->label2->setText(tr("difference\nin %"));
}

void Limits::setLabelsTemperature() {
  ui->unit->setText(tr("Temperature"));
  ui->label1->setText(tr("minimum\nin °C"));
  ui->label2->setText(tr("reference\nin °C"));
}

void Limits::setLabelsHumidityBorder() {
  ui->unit->setText(tr("Humidity Border"));
  ui->label1->setText(tr("border\nin %"));
  ui->label2->setText(tr("difference\nin %"));
}

void Limits::setLabelsSproutValue() {
  ui->unit->setText(tr("Air Quality Mold"));
  ui->label1->setText(tr("border\nin %"));
  ui->label2->setText(tr("difference\nin %"));
}

void Limits::setLabelsVOC() {
  ui->unit->setText(tr("Air Quality VOC"));
  ui->label1->setText(tr("border\nin ppb"));
  ui->label2->setText(tr("difference\nin ppb"));
}

void Limits::setLabelsOutdoorTemperature() {
  ui->unit->setText(tr("Outdoor Temperature"));
  ui->label1->setText(tr("temperature\nin °C"));
}

void Limits::setValuesMax() {
  ui->value1->setMinimum(20);
  ui->value1->setMaximum(80);
  ui->value1->setDecimals(0);
  ui->value1->setSingleStep(5);
  ui->value1->setValue(60);
}

void Limits::setValuesDRH() {
  ui->value2->setMinimum(5);
  ui->value2->setMaximum(20);
  ui->value2->setDecimals(0);
  ui->value2->setSingleStep(5);
  ui->value2->setValue(10);
}

void Limits::setValuesMin() {
  ui->value1->setMinimum(10);
  ui->value1->setMaximum(40);
  ui->value1->setDecimals(1);
  ui->value1->setSingleStep(0.5);
  ui->value1->setValue(18);
}

void Limits::setValuesReference() {
  ui->value2->setMinimum(10);
  ui->value2->setMaximum(40);
  ui->value2->setDecimals(1);
  ui->value2->setSingleStep(0.5);
  ui->value2->setValue(22);
}

void Limits::setValuesHumidityBorder() {
  ui->value1->setMinimum(20);
  ui->value1->setMaximum(80);
  ui->value1->setDecimals(0);
  ui->value1->setSingleStep(5);
  ui->value1->setValue(70);
}

void Limits::setValuesSproutValue() {
  ui->value1->setMinimum(20);
  ui->value1->setMaximum(90);
  ui->value1->setDecimals(0);
  ui->value1->setSingleStep(10);
  ui->value1->setValue(50);
}

void Limits::setValuesSproutValueDifference() {
  ui->value2->setMinimum(5);
  ui->value2->setMaximum(20);
  ui->value2->setDecimals(0);
  ui->value2->setSingleStep(5);
  ui->value2->setValue(10);
}

void Limits::setValuesVOC() {
  ui->value1->setMinimum(50);
  ui->value1->setMaximum(1000);
  ui->value1->setDecimals(0);
  ui->value1->setSingleStep(50);
  ui->value1->setValue(100);
}

void Limits::setValuesVOCDifference() {
  ui->value2->setMinimum(10);
  ui->value2->setMaximum(100);
  ui->value2->setDecimals(0);
  ui->value2->setSingleStep(10);
  ui->value2->setValue(20);
}

void Limits::setValuesOutdoorTemperature() {
  ui->value1->setMinimum(-10);
  ui->value1->setMaximum(40);
  ui->value1->setDecimals(1);
  ui->value1->setSingleStep(0.5);
  ui->value1->setValue(10);
}

void Limits::handleTypeSet() {
  switch (m_type) {
    case LimitTypes::humidity:
      m_limits.humidity.max = static_cast<float>(ui->value1->value());
      m_limits.humidity.drH = static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::humidityBorder:
      m_limits.humidity_border = static_cast<float>(ui->value1->value());
      m_limits.humidity.drH = static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::temperature:
      m_limits.temperature_.min = static_cast<float>(ui->value1->value());
      m_limits.temperature_.reference = static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::CO2:
      m_limits.air_quality_.co2 = static_cast<float>(ui->value1->value());
      m_limits.air_quality_.co2_difference =
          static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::sproutValue:
      m_limits.air_quality_.sprout_value =
          static_cast<float>(ui->value1->value());
      m_limits.air_quality_.sprout_value_difference =
          static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::VOC:
      m_limits.air_quality_.voc = static_cast<float>(ui->value1->value());
      m_limits.air_quality_.voc_difference =
          static_cast<float>(ui->value2->value());
      break;
    case LimitTypes::outdoorTemperature:
      m_limits.outdoor_temperature = static_cast<float>(ui->value1->value());
      break;
  }
}

void Limits::setLimits(std::function<void()> callOnFinished) {
  if (!checkIfChanged()) {
    callOnFinished();
    return;
  }
  handleTypeSet();
  m_services.service->post([controller_handler = m_services.controller_handler,
                            room_id = m_room_id, limits = m_limits,
                            callOnFinished]() {
    controller_handler->set_limits(room_id, limits);
    postToGUIThread([callOnFinished] { callOnFinished(); });
  });
}

void Limits::handleLimitsLoaded() {
  wolf::thread_checker::throw_if_not_ui(
      "ControllerSettings::handleStatesLoaded");
  switch (m_type) {
    case LimitTypes::humidity:
      ui->value1->setValue(static_cast<double>(m_limits.humidity.max));
      ui->value2->setValue(static_cast<double>(m_limits.humidity.drH));
      break;
    case LimitTypes::humidityBorder:
      ui->value1->setValue(static_cast<double>(m_limits.humidity_border));
      ui->value2->setValue(static_cast<double>(m_limits.humidity.drH));
      break;
    case LimitTypes::temperature:
      ui->value1->setValue(static_cast<double>(m_limits.temperature_.min));
      ui->value2->setValue(
          static_cast<double>(m_limits.temperature_.reference));
      break;
    case LimitTypes::CO2:
      ui->value1->setValue(static_cast<double>(m_limits.air_quality_.co2));
      ui->value2->setValue(
          static_cast<double>(m_limits.air_quality_.co2_difference));
      break;
    case LimitTypes::sproutValue:
      ui->value1->setValue(
          static_cast<double>(m_limits.air_quality_.sprout_value));
      ui->value2->setValue(
          static_cast<double>(m_limits.air_quality_.sprout_value_difference));
      break;
    case LimitTypes::VOC:
      ui->value1->setValue(static_cast<double>(m_limits.air_quality_.voc));
      ui->value2->setValue(
          static_cast<double>(m_limits.air_quality_.voc_difference));
      break;
    case LimitTypes::outdoorTemperature:
      ui->value1->setValue(static_cast<double>(m_limits.outdoor_temperature));
      break;
  }
}

void Limits::loadLimits() {
  m_limits = m_dataExchange.get_limits(m_room_id);
  handleLimitsLoaded();
  auto valueButtons_ = makeValueButtons();
  ButtonDisableHandler::checkAllDisabledValues(valueButtons_);
}

void Limits::saveOldLimits() {
  m_old_limits[0] = static_cast<float>(ui->value1->value());
  m_old_limits[1] = static_cast<float>(ui->value2->value());
}

bool Limits::checkIfChanged() const {
  return (!wolf::math::compare_floats(
              m_old_limits[0], static_cast<float>(ui->value1->value())) ||
          !wolf::math::compare_floats(m_old_limits[1],
                                      static_cast<float>(ui->value2->value())));
}

Limits::valueButtons Limits::makeValueButtons() {
  valueButtons valueButtons_;
  valueButtons_.push_back({ui->value1, ui->value1->minimum(), {ui->lower1}});
  valueButtons_.push_back({ui->value1, ui->value1->maximum(), {ui->raise1}});
  valueButtons_.push_back({ui->value2, ui->value2->minimum(), {ui->lower2}});
  valueButtons_.push_back({ui->value2, ui->value2->maximum(), {ui->raise2}});
  return valueButtons_;
}
