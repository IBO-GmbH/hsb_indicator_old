#include "HeatingValveSettings.hpp"
#include "ButtonDisableHandler.hpp"
#include "PostToGUIThread.hpp"
#include "mold/heating_valve_helper.hpp"
#include "ui_HeatingValveSettings.h"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/thread_checker.hpp"

#include <QTimer>

mold::HeatingValveSettings::HeatingValveSettings(
    const mold::WolfServices &services, const wolf::room &room, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HeatingValveSettings),
      m_logger("HeatingValveSettings"),
      m_services(services),
      m_room(room),
      m_current_valve_index(0) {
  wolf::thread_checker::throw_if_not_ui(
      "HeatingValveSettings::HeatingValveSettings");

  ui->setupUi(this);
  ui->labelDisplayedValve->setVisible(false);

  m_connected_valves =
      heating_valve_helper::get_all_heating_valve_enocean_ids(room.sensors);
  m_sensor_ids =
      heating_valve_helper::get_all_heating_valve_sensor_ids(room.sensors);

  m_connection_values = m_services.value_handler->signal_values.connect(
      [this](const wolf::sensor_values &values) {
        postToGUIThread([this, values] { updateValues(values); });
      });

  connect(ui->back, &QPushButton::clicked, this,
          [this] { saveSettings([this] { emit back(); }); });
  connect(ui->lower, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleLower(ui->temperatureValue, {ui->lower},
                                      {ui->raise});
  });
  connect(ui->raise, &QPushButton::clicked, this, [this] {
    ButtonDisableHandler::handleRaise(ui->temperatureValue, {ui->lower},
                                      {ui->raise});
  });
  connect(ui->enableManualPosition, &QCheckBox::toggled, this,
          &HeatingValveSettings::enableManualMode);
  connect(ui->valvePositionSlider, &QAbstractSlider::valueChanged, this,
          &HeatingValveSettings::setSliderValue);
  connect(ui->learnValve, &QPushButton::clicked, this,
          [this] { saveSettings([this] { emit openLearnPage(); }); });

  loadSettings();
  loadValues();
  handleCurrentValveDisplay();
}

std::optional<mold::heating_valve_settings>
mold::HeatingValveSettings::get_current_settings() const {
  return m_settings;
}

mold::HeatingValveSettings::~HeatingValveSettings() = default;

void mold::HeatingValveSettings::enableManualMode() {
  const auto manual_checked = ui->enableManualPosition->isChecked();
  ui->valvePositionSlider->setEnabled(manual_checked);
  ui->valvePositionValue->setEnabled(manual_checked);
  ui->temperatureValue->setEnabled(!manual_checked);
  ui->lower->setEnabled(!manual_checked);
  ui->raise->setEnabled(!manual_checked);
}

void mold::HeatingValveSettings::setSliderValue(const int value) {
  ui->valvePositionValue->setValue(value);
}

void mold::HeatingValveSettings::handleCurrentValveDisplay() {
  const auto number_of_valves = m_connected_valves.size();
  if (number_of_valves <= 1) return;
  ui->labelDisplayedValve->setVisible(true);
  ui->labelDisplayedValve->setNum(static_cast<int>(m_current_valve_index + 1));
  setValuesToUI();
  QTimer::singleShot(3000, this,
                     &HeatingValveSettings::handleCurrentValveDisplay);
  ++m_current_valve_index;
  if (m_current_valve_index >= number_of_valves) m_current_valve_index = 0;
}

void mold::HeatingValveSettings::saveSettings(
    std::function<void()> callOnFinished) {
  setDisabled(true);
  auto settings = getSettingsFromUI();
  if (m_settings.has_value() && settings == m_settings.value()) {
    callOnFinished();
    return;
  }
  m_settings = settings;
  m_services.service->post([this,
                            handler = m_services.heating_valve_settings_handler,
                            settings, callOnFinished]() {
    try {
      handler->set_options(settings);
    } catch (std::runtime_error &error) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "failed to set settings, error:" << error.what();
    }
    postToGUIThread([callOnFinished] { callOnFinished(); });
  });
}

void mold::HeatingValveSettings::loadSettings() {
  setDisabled(true);
  m_services.service->post(
      [this, handler = m_services.heating_valve_settings_handler] {
        const auto settings = handler->get_options_for_room(m_room);
        postToGUIThread([this, settings] {
          m_settings = settings;
          if (settings.has_value()) {
            ui->enableManualPosition->setChecked(settings->manual_mode);
            ui->enableSummerMode->setChecked(settings->summer_mode);
            ui->temperatureValue->setValue(
                static_cast<double>(settings->desired_temperature));
            ui->valvePositionSlider->setValue(settings->set_valve_position);
          }
          handleLoadingFinished();
        });
      });
}

mold::heating_valve_settings mold::HeatingValveSettings::getSettingsFromUI()
    const {
  heating_valve_settings new_settings;
  new_settings.room_id = m_room.id;
  new_settings.manual_mode = ui->enableManualPosition->isChecked();
  new_settings.summer_mode = ui->enableSummerMode->isChecked();
  new_settings.desired_temperature =
      static_cast<float>(ui->temperatureValue->value());
  new_settings.set_valve_position = ui->valvePositionSlider->value();
  return new_settings;
}

void mold::HeatingValveSettings::handleLoadingFinished() {
  enableManualMode();
  ButtonDisableHandler::checkAllDisabledValues(
      {{ui->temperatureValue, ui->temperatureValue->minimum(), {ui->lower}},
       {ui->temperatureValue, ui->temperatureValue->maximum(), {ui->raise}}});
  setDisabled(false);
}

void mold::HeatingValveSettings::loadValues() {
  m_services.service->post([this, value_handler = m_services.value_handler]() {
    wolf::sensor_values sensor_values;
    for (const auto &id : m_sensor_ids) {
      const auto value = value_handler->get_value(id);
      if (!value.has_value()) continue;
      sensor_values.push_back(value.value());
    }
    postToGUIThread([this, sensor_values] { updateValues(sensor_values); });
  });
}

void mold::HeatingValveSettings::updateValues(
    const wolf::sensor_values &values) {
  bool change = values.empty();
  for (const auto &value : values) {
    if (!heating_valve_helper::is_heating_valve(value.id)) continue;
    auto found = std::find_if(m_values.begin(), m_values.end(),
                              [&value](const wolf::sensor_value &check) {
                                return value.id == check.id;
                              });
    if (found != m_values.end()) {
      if (wolf::math::compare_floats(found->value, value.value)) continue;
      *found = value;
      change = true;
      continue;
    }
    m_values.push_back(value);
    change = true;
  }
  if (change) setValuesToUI();
}

void mold::HeatingValveSettings::setValuesToUI() {
  ui->labelCurrentValue->setText("--");
  const bool manual_position = ui->enableManualPosition->isChecked();
  if (!manual_position) ui->valvePositionSlider->setValue(0);
  const auto values_to_set = heating_valve_helper::get_heating_valve_values(
      m_values, m_connected_valves[m_current_valve_index]);
  for (const auto &value : values_to_set) {
    if (value.id.get_type() == wolf::sensor_value_type::valve_temperature)
      ui->labelCurrentValue->setText(
          QString::number(static_cast<double>(value.value), 'f', 1));
    else if (!manual_position &&
             value.id.get_type() == wolf::sensor_value_type::valve_position)
      ui->valvePositionSlider->setValue(static_cast<int>(value.value));
  }
}
