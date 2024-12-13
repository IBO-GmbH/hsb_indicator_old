#include "GenericValueForm.hpp"
#include <QToolButton>
#include "SensorValueTypeToText.hpp"
#include "ValueCheckHelper.hpp"
#include "ui_GenericValueForm.h"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_value_type_helper.hpp"

using namespace logging;
using namespace mold;

GenericValueForm::GenericValueForm(statusForm::constructorItems &items,
                                   const wolf::sensor_id &sensor_id,
                                   QWidget *parent)
    : StatusFormBase(parent),
      m_logger("GenericValueForm"),
      ui(new Ui::GenericValueForm),
      m_services(items.services),
      m_dataExchange(items.dataExchange),
      m_room_id(items.room_id),
      m_sensor_id(sensor_id),
      m_sensor_grey_time(std::chrono::minutes(
          m_services.config_wolf->general_.sensor_values_grey_time)) {
  ui->setupUi(this);

  LOGGING_LOG(m_logger, severity::verbose)
      << "GenericValueForm() room_id:" << m_room_id
      << " sensor id:" << sensor_id;

  connect(ui->myLovelyButton, &InvisibleClickableButton::pressed,
          [this]() { ui->statusBar->getSettingsButton()->setDown(true); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::released,
          [this]() { ui->statusBar->getSettingsButton()->setDown(false); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::clicked,
          ui->statusBar->getSettingsButton(), &QToolButton::click);

  connect(&m_dataExchange, &DataExchange::generic_values, this,
          &GenericValueForm::updateOutput);

  ui->motion->setVisible(false);
  ui->statusBar->initialise(m_services, items.gsmInformationReader);
  setDescriptors();
  updateOutput();

  ui->header->initialise(m_room_id, m_dataExchange, m_services,
                         items.powerHandler, items.rotation, false);
}

GenericValueForm::~GenericValueForm() = default;

StatusBarForm *GenericValueForm::getStatusBar() const { return ui->statusBar; }

RoomHeaderForm *GenericValueForm::getRoomHeader() const { return ui->header; }

void GenericValueForm::updateOutput() {
  handleMotion();
  auto value = getValue();
  if (!value) {
    LOGGING_LOG(m_logger, severity::warning) << "getValue, delivered no result,"
                                                " m_sensor_id:"
                                             << m_sensor_id;

    ValueCheckHelper::setNoData(*ui->generic_value);
    return;
  }

  LOGGING_LOG(m_logger, severity::verbose)
      << "getValue, delivered: " << value.get()
      << " , m_sensor_id : " << m_sensor_id;
  QLocale local;
  const auto round_value = std::floor(value.get() * 100) / 100;
  ui->generic_value->setText(local.toString(round_value, 'g', 6));
}

void GenericValueForm::setDescriptors() {
  auto description = SensorValueTypeToText::description(m_sensor_id.get_type());
  if (!description.has_value()) description = tr("Unknown");
  auto unit = SensorValueTypeToText::unit(m_sensor_id.get_type());
  if (!unit.has_value()) unit = tr("Unknown");
  ui->generic_title->setText(description.value());
  ui->generic_unit->setText(unit.value());
}

boost::optional<float> GenericValueForm::getValue() const {
  const auto values = m_dataExchange.get_values();
  auto sensor_id = m_sensor_id;
  const auto found =
      std::find_if(values.cbegin(), values.cend(),
                   [&sensor_id](const wolf::sensor_value &toCheck) {
                     return toCheck.id == sensor_id;
                   });
  if (found == values.cend()) {
    return {};
  }
  const bool old = ValueCheckHelper::checkIfOld(
      found->timestamp, std::chrono::system_clock::now(), m_sensor_grey_time);
  ValueCheckHelper::changeColourIfOld({ui->generic_value, ui->generic_unit},
                                      old);
  return found->value;
}

void GenericValueForm::handleMotion() {
  const auto room = m_dataExchange.get_room(m_room_id);
  if (!room.has_value()) return;
  const auto sensors = room->sensors;
  const auto found = std::find_if(
      sensors.cbegin(), sensors.cend(), [](const wolf::sensor_id &to_check) {
        return wolf::sensor_value_type_helper::is_motion(to_check) ||
               wolf::sensor_value_type_helper::is_presence(to_check);
      });
  if (found == sensors.cend()) return;
  handleIcon(*found);
  const auto value = m_services.value_handler->get_value_float(*found);
  if (!value.has_value()) return;
  const bool motion = wolf::math::compare_floats(value.get(), 1.f);
  ui->motion->setVisible(motion);
}

void GenericValueForm::handleIcon(const wolf::sensor_id &type) {
  if (wolf::sensor_value_type_helper::is_motion(type))
    ui->motion->setStyleSheet("image: url(:/settings/icons/motion.svg);");
  else if (wolf::sensor_value_type_helper::is_presence(type))
    ui->motion->setStyleSheet("image: url(:/settings/icons/presence.svg);");
}
