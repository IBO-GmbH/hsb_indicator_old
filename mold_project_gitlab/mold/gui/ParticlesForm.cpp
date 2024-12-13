#include "ParticlesForm.hpp"
#include <QToolButton>
#include "ValueCheckHelper.hpp"
#include "ui_ParticlesForm.h"

using namespace logging;

mold::ParticlesForm::ParticlesForm(statusForm::constructorItems &items,
                                   const sensors &sensor_ids, QWidget *parent)
    : StatusFormBase(parent),
      m_logger("ParticlesForm"),
      ui(new Ui::ParticlesForm),
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
          &ParticlesForm::updateOutput);

  ui->statusBar->initialise(items.services, items.gsmInformationReader);
  updateOutput();

  ui->header->initialise(items.room_id, m_dataExchange, items.services,
                         items.powerHandler, items.rotation, false);
}

mold::StatusBarForm *mold::ParticlesForm::getStatusBar() const {
  return ui->statusBar;
}

mold::RoomHeaderForm *mold::ParticlesForm::getRoomHeader() const {
  return ui->header;
}

void mold::ParticlesForm::updateOutput() {
  if (m_sensor_ids.size() != 3) {
    LOGGING_LOG(m_logger, severity::warning)
        << "there are not 3 particle sensors";
    return;
  }
  auto values = getValues();
  const std::vector<QLabel *> labels{ui->pm10Value, ui->pm2p5Value,
                                     ui->pm1Value};
  for (unsigned int index = 0; index < m_sensor_ids.size(); ++index) {
    if (!values[index].has_value()) {
      LOGGING_LOG(m_logger, severity::warning)
          << "getValue, delivered no result,"
             " m_sensor_id: "
          << m_sensor_ids[index];

      ValueCheckHelper::setNoData(*labels[index]);
      continue;
    }

    LOGGING_LOG(m_logger, severity::verbose)
        << "getValue, delivered: " << values[index].get()
        << " , m_sensor_id : " << m_sensor_ids[index];
    labels[index]->setText(
        QString::number(std::lround(values[index].get().value)));
    const bool old = ValueCheckHelper::checkIfOld(
        values[index].get().timestamp, std::chrono::system_clock::now(),
        m_sensor_grey_time);
    ValueCheckHelper::changeColourIfOld({labels[index]}, old);
  }
}

mold::ParticlesForm::values mold::ParticlesForm::getValues() const {
  const auto all_values = m_dataExchange.get_values();
  values particle_values;
  for (unsigned int index = 0; index < m_sensor_ids.size(); ++index) {
    const auto found = std::find_if(
        all_values.cbegin(), all_values.cend(),
        [sensor_id = m_sensor_ids[index]](const wolf::sensor_value &toCheck) {
          return toCheck.id == sensor_id;
        });
    if (found != all_values.cend()) {
      particle_values[index] = *found;
      continue;
    }
    particle_values[index] = boost::none;
  }
  return particle_values;
}

mold::ParticlesForm::~ParticlesForm() = default;
