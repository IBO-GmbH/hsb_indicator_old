#include "CurrentClampForm.hpp"
#include <QDateTime>
#include <QToolButton>
#include "ValueCheckHelper.hpp"
#include "ui_CurrentClampForm.h"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;

mold::CurrentClampForm::CurrentClampForm(statusForm::constructorItems &items,
                                         const sensors &sensor_ids,
                                         QWidget *parent)
    : StatusFormBase(parent),
      m_logger("CurrentClampForm"),
      ui(new Ui::CurrentClampForm),
      m_dataExchange(items.dataExchange),
      m_sensor_ids(sensor_ids),
      m_sensor_grey_time(std::chrono::minutes(
          items.services.config_wolf->general_.sensor_values_grey_time)) {
  ui->setupUi(this);

  connect(ui->myLovelyButton, &InvisibleClickableButton::pressed,
          [this]() { ui->statusBar->getSettingsButton()->setDown(true); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::released,
          [this]() { ui->statusBar->getSettingsButton()->setDown(false); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::clicked,
          ui->statusBar->getSettingsButton(), &QToolButton::click);

  connect(&m_dataExchange, &DataExchange::generic_values, this,
          &CurrentClampForm::updateOutput);

  connect(ui->energyReset, &QPushButton::clicked, this,
          [service = items.services.service,
           power_calculator = items.services.power_calculator_, sensor_ids] {
            service->post([&power_calculator, sensor_ids]() {
              power_calculator->reset_energy(sensor_ids);
            });
          });

  ui->statusBar->initialise(items.services, items.gsmInformationReader);
  updateOutput();

  ui->header->initialise(items.room_id, m_dataExchange, items.services,
                         items.powerHandler, items.rotation, false);
}

mold::CurrentClampForm::~CurrentClampForm() = default;

mold::StatusBarForm *mold::CurrentClampForm::getStatusBar() const {
  return ui->statusBar;
}

mold::RoomHeaderForm *mold::CurrentClampForm::getRoomHeader() const {
  return ui->header;
}

void mold::CurrentClampForm::updateOutput() {
  auto values = getValues();
  QLocale local;
  const std::vector<QLabel *> labels{
      ui->current1,         ui->current2,       ui->current3,
      ui->currentMomentary, ui->power1,         ui->power2,
      ui->power3,           ui->powerMomentary, ui->energy};
  for (const auto &sensor : m_sensor_ids) {
    const auto index = sensor.get_id().back();
    if (!values[index].has_value()) {
      LOGGING_LOG(m_logger, severity::warning)
          << "getValue, delivered no result, m_sensor_id: " << sensor;
      ValueCheckHelper::setNoData(*labels[index]);
      continue;
    }

    auto value = values[index].get().value;
    value = std::floor(value * 10) / 10;
    value = std::min(value, 99999.9f);
    if (m_old_values[index].has_value() &&
        wolf::math::compare_floats(m_old_values[index].value(), value))
      continue;
    m_old_values[index] = value;
    LOGGING_LOG(m_logger, severity::verbose)
        << "getValue, delivered: " << values[index].get()
        << " , m_sensor_id : " << sensor;
    labels[index]->setText(local.toString(value, 'g', 6));
    const bool old = ValueCheckHelper::checkIfOld(
        values[index].get().timestamp, std::chrono::system_clock::now(),
        m_sensor_grey_time);
    ValueCheckHelper::changeColourIfOld({labels[index]}, old);
    if (!wolf::sensor_value_type_helper::is_electric_energy(sensor)) continue;
    const auto timestamp =
        m_dataExchange.getServices().power_calculator_->get_start_time_energy(
            sensor);
    if (!timestamp.has_value()) continue;
    QString label{tr("Consumption since ")};
    auto time = std::chrono::system_clock::to_time_t(timestamp.value());
    auto date_time = QDateTime::fromSecsSinceEpoch(time);
    label += date_time.toString("dd.MM.yyyy");
    ui->labelEnergy->setText(label);
  }
}

mold::CurrentClampForm::values mold::CurrentClampForm::getValues() const {
  const auto all_values = m_dataExchange.get_values();
  values current_and_power_values;
  for (auto &value : current_and_power_values) value = boost::none;
  for (const auto &sensor : m_sensor_ids) {
    const auto found =
        std::find_if(all_values.cbegin(), all_values.cend(),
                     [&sensor](const wolf::sensor_value &toCheck) {
                       return toCheck.id == sensor;
                     });
    if (found != all_values.cend())
      current_and_power_values[sensor.get_id().back()] = *found;
  }
  return current_and_power_values;
}
