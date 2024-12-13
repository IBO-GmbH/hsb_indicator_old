#include "SensorOffsetForm.hpp"
#include "ui_SensorOffsetForm.h"

#include "ButtonDisableHandler.hpp"

using namespace mold;

SensorOffsetForm::SensorOffsetForm(const offsets& offsets_, bool show_second,
                                   QWidget* parent)
    : QWidget(parent), ui(new Ui::SensorOffsetForm) {
  ui->setupUi(this);

  ui->valueTemperature->setValue(static_cast<double>(offsets_.temperature));
  ui->valueHumidity->setValue(static_cast<double>(offsets_.humidity));

  connect(ui->lowerTemperatureSmall, &QPushButton::clicked,
          ui->valueTemperature, [this] { lowerTemperature(); });
  connect(ui->raiseTemperatureSmall, &QPushButton::clicked,
          ui->valueTemperature, [this] { raiseTemperature(); });
  connect(ui->lowerTemperatureBig, &QPushButton::clicked, this,
          [this] { stepTemperature(-10); });
  connect(ui->raiseTemperatureBig, &QPushButton::clicked, this,
          [this] { stepTemperature(10); });

  connect(ui->lowerHumiditySmall, &QPushButton::clicked, ui->valueHumidity,
          [this] { lowerHumidity(); });
  connect(ui->raiseHumiditySmall, &QPushButton::clicked, ui->valueHumidity,
          [this] { raiseHumidity(); });
  connect(ui->lowerHumidityBig, &QPushButton::clicked, this,
          [this] { stepHumidity(-10); });
  connect(ui->raiseHumidityBig, &QPushButton::clicked, this,
          [this] { stepHumidity(10); });

  ui->lowerHumiditySmall->setVisible(show_second);
  ui->raiseHumiditySmall->setVisible(show_second);
  ui->lowerHumidityBig->setVisible(show_second);
  ui->raiseHumidityBig->setVisible(show_second);
  ui->valueHumidity->setVisible(show_second);
  ui->labelHumidity->setVisible(show_second);

  connect(ui->back, &QPushButton::clicked, this, &SensorOffsetForm::back);
  checkDisabled();
}

SensorOffsetForm::~SensorOffsetForm() = default;

SensorOffsetForm::offsets SensorOffsetForm::getOffset() const {
  return {static_cast<float>(ui->valueTemperature->value()),
          static_cast<float>(ui->valueHumidity->value())};
}

SensorOffsetForm::valueButtons SensorOffsetForm::makeValueButtons() {
  valueButtons valueButtons_;
  valueButtons_.push_back(
      {ui->valueTemperature,
       ui->valueTemperature->minimum(),
       {ui->lowerTemperatureSmall, ui->lowerTemperatureBig}});
  valueButtons_.push_back(
      {ui->valueTemperature,
       ui->valueTemperature->maximum(),
       {ui->raiseTemperatureSmall, ui->raiseTemperatureBig}});
  valueButtons_.push_back({ui->valueHumidity,
                           ui->valueHumidity->minimum(),
                           {ui->lowerHumiditySmall, ui->lowerHumidityBig}});
  valueButtons_.push_back({ui->valueHumidity,
                           ui->valueHumidity->maximum(),
                           {ui->raiseHumiditySmall, ui->raiseHumidityBig}});
  return valueButtons_;
}

void SensorOffsetForm::checkDisabled() {
  auto valueButtons_ = makeValueButtons();
  ButtonDisableHandler::checkAllDisabledValues(valueButtons_);
}

void SensorOffsetForm::lowerTemperature() {
  ButtonDisableHandler::handleLower(
      ui->valueTemperature,
      {ui->lowerTemperatureSmall, ui->lowerTemperatureBig},
      {ui->raiseTemperatureSmall, ui->raiseTemperatureBig});
}

void SensorOffsetForm::lowerHumidity() {
  ButtonDisableHandler::handleLower(
      ui->valueHumidity, {ui->lowerHumiditySmall, ui->lowerHumidityBig},
      {ui->raiseHumiditySmall, ui->raiseHumidityBig});
}

void SensorOffsetForm::raiseTemperature() {
  ButtonDisableHandler::handleRaise(
      ui->valueTemperature,
      {ui->lowerTemperatureSmall, ui->lowerTemperatureBig},
      {ui->raiseTemperatureSmall, ui->raiseTemperatureBig});
}

void SensorOffsetForm::raiseHumidity() {
  ButtonDisableHandler::handleRaise(
      ui->valueHumidity, {ui->lowerHumiditySmall, ui->lowerHumidityBig},
      {ui->raiseHumiditySmall, ui->raiseHumidityBig});
}

void SensorOffsetForm::stepTemperature(const int steps) {
  ButtonDisableHandler::handleStep(
      ui->valueTemperature,
      {ui->lowerTemperatureSmall, ui->lowerTemperatureBig},
      {ui->raiseTemperatureSmall, ui->raiseTemperatureBig}, steps);
}

void SensorOffsetForm::stepHumidity(const int steps) {
  ButtonDisableHandler::handleStep(
      ui->valueHumidity, {ui->lowerHumiditySmall, ui->lowerHumidityBig},
      {ui->raiseHumiditySmall, ui->raiseHumidityBig}, steps);
}
