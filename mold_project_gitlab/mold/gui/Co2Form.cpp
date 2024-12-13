#include "Co2Form.hpp"
#include <QToolButton>
#include "ValueCheckHelper.hpp"
#include "ui_Co2Form.h"

using namespace logging;
using namespace mold;

Co2Form::Co2Form(statusForm::constructorItems &items,
                 const wolf::sensor_id &sensor_id,
                 const Co2StateHandler &co2_state, QWidget *parent)
    : StatusFormBase(parent),
      m_logger("Co2Form"),
      ui(new Ui::Co2Form),
      m_dataExchange(items.dataExchange),
      m_room_id(items.room_id),
      m_sensor_id(sensor_id),
      m_co2_state(co2_state),
      m_sensor_grey_time(std::chrono::minutes(
          items.services.config_wolf->general_.sensor_values_grey_time)) {
  ui->setupUi(this);

  connect(ui->myLovelyButton, &InvisibleClickableButton::pressed,
          [this]() { ui->statusBar->getSettingsButton()->setDown(true); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::released,
          [this]() { ui->statusBar->getSettingsButton()->setDown(false); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::clicked,
          ui->statusBar->getSettingsButton(), &QToolButton::click);

  connect(&m_co2_state, &Co2StateHandler::changeCo2Ui, this,
          &Co2Form::handleNewValue);

  ui->statusBar->initialise(items.services, items.gsmInformationReader);

  updateOutput();

  ui->header->initialise(m_room_id, m_dataExchange, items.services,
                         items.powerHandler, items.rotation, true);
}

Co2Form::~Co2Form() = default;

StatusBarForm *Co2Form::getStatusBar() const { return ui->statusBar; }

RoomHeaderForm *Co2Form::getRoomHeader() const { return ui->header; }

void Co2Form::handleNewValue(const wolf::sensor_id &id) {
  if (id != m_sensor_id) return;
  updateOutput();
}

void Co2Form::updateOutput() {
  value value = {0.f, false};

  auto valueOpt = getValue();
  if (valueOpt.is_initialized() && (valueOpt.get().value > 400.f)) {
    value = valueOpt.get();
    value.value = std::min(9999.f, value.value);
    const int value_rounded = std::lround(value.value);
    value.value = value_rounded;
    ui->co2_value->setText(QString::number(value_rounded));
  } else
    ValueCheckHelper::setNoData(*ui->co2_value);

  ui->co2_render->setValue(value.value);

  auto state = m_co2_state.getStateOfRoom(m_room_id);
  if (state == calculation_state::undefined) {
    LOGGING_LOG(m_logger, severity::normal)
        << "state undefined, change to green";
    state = calculation_state::green;
  }
  if (state == m_state) {
    return;
  }
  m_state = state;
  if (m_state == calculation_state::green) {
    setStyle(value, "#66cc00");
    ui->co2_rating->setText(tr("normal"));
  }
  if (calculation_state_is_yellow(m_state)) {
    setStyle(value, "#ff9900");
    setWarningMessage(tr("CO₂ too high!"), tr("Please ventilate."));
    ui->co2_rating->setText(tr("high"));
  }
  if (calculation_state_is_red(m_state)) {
    setStyle(value, "#cc0000");
    setWarningMessage(tr("CO₂ too high!"), tr("Please ventilate."));
    ui->co2_rating->setText(tr("very high"));
  }
  ui->warning_mold->setVisible(m_state != calculation_state::green);
}

void Co2Form::setWarningMessage(const QString &title, const QString &message) {
  ui->warning_mold->setText(
      "<html><head/><body><p><span style=\" font-weight:600;\">" + title +
      "</span><br/>" + message + "</p></body></html>");
}

boost::optional<Co2Form::value> Co2Form::getValue() const {
  const auto values = m_dataExchange.get_values();
  const auto found = std::find_if(values.cbegin(), values.cend(),
                                  [this](const wolf::sensor_value &toCheck) {
                                    return toCheck.id == m_sensor_id;
                                  });
  if (found == values.cend()) return {};

  value sensor_value;
  sensor_value.old = ValueCheckHelper::checkIfOld(
      found->timestamp, std::chrono::system_clock::now(), m_sensor_grey_time);
  ValueCheckHelper::changeColourIfOld({ui->co2_value, ui->co2_unit},
                                      sensor_value.old);
  sensor_value.value = found->value;
  return sensor_value;
}

void Co2Form::setStyle(const Co2Form::value &value_, const QString &colour) {
  if (value_.old)
    setStyleSheet(makeStyleSheetOld(colour));
  else
    setStyleSheet(makeStyleSheet(colour));
}

QString Co2Form::makeStyleSheetOld(const QString &colour) {
  QString style_sheet("#roomNameContainer {background-color: ");
  style_sheet += colour;
  style_sheet += ";}";
  return style_sheet;
}

QString Co2Form::makeStyleSheet(const QString &colour) {
  QString style_sheet(makeStyleSheetOld(colour));
  style_sheet += "#co2_value {color: ";
  style_sheet += colour;
  style_sheet += ";}#co2_unit {color: ";
  style_sheet += colour;
  style_sheet += ";}";
  return style_sheet;
}
