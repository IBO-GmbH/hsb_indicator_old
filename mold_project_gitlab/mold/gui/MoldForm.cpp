#include "MoldForm.hpp"
#include "ui_MoldForm.h"

#include "ValueCheckHelper.hpp"
#include "mold/gui/MoldValuesForm.hpp"
#include "mold/program_options_parser.hpp"
#include "wolf/actor/actor_indexes.hpp"
#include "wolf/math_compare_decimals.hpp"
#include "wolf/sensor_value_type_helper.hpp"

#include <QToolButton>

using namespace logging;
using namespace mold;

MoldForm::MoldForm(statusForm::constructorItems &items, QWidget *parent)
    : StatusFormBase(parent),
      ui(new Ui::MoldForm),
      m_logger{"MoldForm"},
      m_services(items.services),
      m_dataExchange(items.dataExchange),
      m_room_id(items.room_id) {
  ui->setupUi(this);  // brings up [qt] QObject::connect: invalid null parameter

  m_moldValues =
      new MoldValuesForm(m_services, m_dataExchange, m_room_id, this);
  m_moldValues->move(0, ui->header->getHeightNameLabel());

  connect(ui->myLovelyButton, &InvisibleClickableButton::pressed,
          [this]() { ui->statusBar->getSettingsButton()->setDown(true); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::released,
          [this]() { ui->statusBar->getSettingsButton()->setDown(false); });
  connect(ui->myLovelyButton, &InvisibleClickableButton::clicked,
          ui->statusBar->getSettingsButton(), &QToolButton::click);
  connect(&m_dataExchange, &DataExchange::configuration_states_changed, this,
          [this] { updateOutputWithCatch(); });
  connect(&m_dataExchange, &DataExchange::configuration_values_changed, this,
          [this] { updateOutputWithCatch(); });
  connect(&m_dataExchange, &DataExchange::controller_updated, this,
          &MoldForm::updateControllerView);
  connect(m_moldValues, &MoldValuesForm::openHeatingValveOptions, this,
          &StatusFormBase::openDifferentPage);
  ui->statusBar->initialise(m_services, items.gsmInformationReader);

  m_state.state = mold::calculation_state::undefined;

  ui->warning_mold->setVisible(false);
  ui->actor_1->setVisible(false);
  ui->actor_2->setVisible(false);
  ui->actor_3->setVisible(false);
  ui->actor_4->setVisible(false);
  ui->controllerName->setText("");
  ui->windowClosed->setVisible(false);
  ui->windowOpen->setVisible(false);

  try {
    updateControllerView();
    updateOutput();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not update, error:" << error.what();
  }

  // ensure m_state.state got set!
  ui->header->initialise(m_room_id, m_dataExchange, m_services,
                         items.powerHandler, items.rotation, false);
}

MoldForm::~MoldForm() = default;

void MoldForm::updateOutput() {
  const boost::optional<configuration> config_check =
      m_dataExchange.get_configuration_by_room(m_room_id);
  if (!config_check) {
    LOGGING_LOG(m_logger, severity::warning)
        << "updateOutput(): no configuration associated with the room:"
        << m_room_id;
    throw std::runtime_error(
        "MoldForm::updateOutput: no configuration associated with room");
  }
  m_configuration = config_check.get();
  auto config_id = m_configuration.id;
  if (config_id == wolf::types::uuid_array{}) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to update output because config not found!";
    throw std::runtime_error(
        "MoldForm::updateOutput: failed to update output, config not found");
  }
  // get state
  const auto state = m_dataExchange.get_configuration_state(config_id);
  if (!state) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to update output because state not found! config_id:"
        << config_id;
    throw std::runtime_error(
        "MoldForm::updateOutput: failed to update output, state not found");
  }
  bool did_calculation_state_change{false};
  if (m_state != state.get()) {
    did_calculation_state_change = m_state.state != state.get().state;
    m_state = state.get();
  }

  handleWindow();
  updateStateUI(did_calculation_state_change);
}

void MoldForm::updateStateUI(const bool did_calculation_state_change) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateStateUI(), room:" << m_state
      << ", did_calculation_state_change:" << std::boolalpha
      << did_calculation_state_change;
  const bool indoorHumidityBiggerThanOutdoor = [this] {
    const auto &indoor = m_moldValues->getIndoorValues().indoor_humidity;
    const auto &outdoor = m_moldValues->getOutdoorValues().humidity;
    if ((!indoor) || (!outdoor)) return true;
    const auto now = std::chrono::system_clock::now();
    if (!ValueCheckHelper::isValueValid(outdoor, now, m_calculation_timeout))
      return true;
    return indoor.get().value_ > outdoor.get().value_;
  }();

  ui->warning_mold->setVisible(m_state.state != calculation_state::green &&
                               indoorHumidityBiggerThanOutdoor);
  const auto now = std::chrono::system_clock::now();
  if (m_state.state == calculation_state::green) {
    bool messageAfterNotGreenVisible{false};
    if (m_state.time_since_green) {
      const auto time_diff_not_red = now - m_state.time_since_green.get();
      messageAfterNotGreenVisible =
          time_diff_not_red <
          std::chrono::seconds(
              m_services.config->mold_states_.not_red_message_timeout);
    }
    ui->warning_mold->setVisible(messageAfterNotGreenVisible);
  }
  if (!did_calculation_state_change) return;
  LOGGING_LOG(m_logger, severity::verbose)
      << "updateStateUI(), advanced, changing style (slow!)";
  if (m_state.state == calculation_state::green) {
    setStyleSheet(
        "#roomNameContainer {"
        "background-color: #66cc00;"
        "}");
  }
  if (calculation_state_is_yellow(m_state.state)) {
    setStyleSheet(
        "#roomNameContainer {"
        "background-color: #ff9900;"
        "}");
    setWarningMessage(tr("Danger of mold!"), tr("Please ventilate."));
  }
  if (calculation_state_is_red(m_state.state)) {
    setStyleSheet(
        "#roomNameContainer {"
        "background-color: #cc0000;"
        "}");
    setWarningMessage(tr("WARNING"), tr("Mold formation possible."));
  }
  if (m_state.state == calculation_state::green) {
    if (m_services.config->mold_states_.not_red_message_timeout > 0) {
      setWarningMessage(tr("WARNING"), tr("Please keep ventilating."));
    }
  }
}

StatusBarForm *MoldForm::getStatusBar() const {
  if (ui->statusBar == nullptr)
    throw std::runtime_error("MoldForm::getStatusBar: status bar not found");
  return ui->statusBar;
}

RoomHeaderForm *MoldForm::getRoomHeader() const {
  if (ui->header == nullptr)
    throw std::runtime_error("MoldForm::getRoomHeader: header not found");
  return ui->header;
}

void MoldForm::updateOutputWithCatch() {
  try {
    updateOutput();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "could not updateOutput, error:" << error.what();
  }
}

void MoldForm::setWarningMessage(const QString &head, const QString &text) {
  ui->warning_mold->setText(
      "<html><head/><body><p><span style=\" font-weight:600;\">" + head +
      "</span><br/>" + text + "</p></body></html>");
}

void MoldForm::updateControllerView() {
  optionalType type;
  if (!checkIfActive(type) || !type) return;
  setActors(type.get());
  handleActors(type.get());
}

bool MoldForm::checkIfActive(boost::optional<ControllerTypes> &type) {
  auto active_states = m_dataExchange.get_active_states(m_room_id);
  bool active = false;
  for (auto &state : active_states)
    if (state) active = true;
  if (!active) {
    ui->controller->setVisible(false);
    return false;
  }
  type = setControllerType(active_states);
  return true;
}

MoldForm::optionalType MoldForm::setControllerType(
    const wolf::controller::values_per_room::active_states &active_states) {
  if (active_states[0]) {
    ui->controllerName->setText(tr("Ventil."));
    return ControllerTypes::sVAS1;
  }
  if (active_states[1]) {
    ui->controllerName->setText(tr("Dehum."));
    return ControllerTypes::sVAS2;
  }
  if (active_states[2]) {
    ui->controllerName->setText(tr("Heating"));
    return ControllerTypes::sVAS3;
  }
  if (active_states[3]) {
    ui->controllerName->setText(tr("Air Qual."));
    return ControllerTypes::sVAS4;
  }
  return boost::none;
}

void MoldForm::setActors(const ControllerTypes &type) {
  switch (type) {
    case ControllerTypes::sVAS1:
      setActorsVentilation();
      break;
    case ControllerTypes::sVAS2:
      setActorsDehumidifier();
      break;
    case ControllerTypes::sVAS3:
      setActorsHeating();
      break;
    case ControllerTypes::sVAS4:
      setActorsVentilation();
      break;
  }
}

void MoldForm::setActorsVentilation() {
  ui->actor_1->setText(tr("Vent."));
  ui->actor_2->setText(tr("Window 1"));
  ui->actor_3->setText(tr("Window 2"));
  ui->actor_4->setText(tr("Window 3"));
}

void MoldForm::setActorsDehumidifier() {
  setActorsVentilation2();
  ui->actor_2->setText(tr("Dehum."));
}

void MoldForm::setActorsHeating() {
  setActorsVentilation2();
  ui->actor_2->setText(tr("Heating"));
}

void MoldForm::setActorsVentilation2() {
  ui->actor_1->setText(tr("Vent."));
  ui->actor_3->setText(tr("Window 1"));
  ui->actor_4->setText(tr("Window 2"));
}

void MoldForm::handleActors(const ControllerTypes &type) {
  auto actors = m_dataExchange.get_actors(m_room_id);
  switch (type) {
    case ControllerTypes::sVAS1:
      handleActorsSVAS1(actors);
      break;
    case ControllerTypes::sVAS2:
      handleActorsSVAS2(actors);
      break;
    case ControllerTypes::sVAS3:
      handleActorsSVAS3(actors);
      break;
    case ControllerTypes::sVAS4:
      handleActorsSVAS4(actors);
      break;
  }
}

void MoldForm::handleActorsSVAS1(
    const wolf::controller::controller::actor_array &actors) {
  bool actor_active = false;
  actor_active =
      handleLabel(actors[wolf::actor::ventilation_svas1], ui->actor_1);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_1_svas1], ui->actor_2);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_2_svas1], ui->actor_3);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_3_svas1], ui->actor_4);
  if (!actor_active) setBlack(ui->controllerName);
}

void MoldForm::handleActorsSVAS2(
    const wolf::controller::controller::actor_array &actors) {
  bool actor_active = false;
  actor_active =
      handleLabel(actors[wolf::actor::ventilation_svas2], ui->actor_1);
  actor_active |=
      handleLabel(actors[wolf::actor::dehumidifier_svas2], ui->actor_2);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_1_svas2], ui->actor_3);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_2_svas2], ui->actor_4);
  if (!actor_active) setBlack(ui->controllerName);
}

void MoldForm::handleActorsSVAS3(
    const wolf::controller::controller::actor_array &actors) {
  bool actor_active = false;
  actor_active =
      handleLabel(actors[wolf::actor::ventilation_svas3], ui->actor_1);
  actor_active |= handleLabel(actors[wolf::actor::heating_svas3], ui->actor_2);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_1_svas3], ui->actor_3);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_2_svas3], ui->actor_4);
  if (!actor_active) setBlack(ui->controllerName);
}

void MoldForm::handleActorsSVAS4(
    const wolf::controller::controller::actor_array &actors) {
  bool actor_active = false;
  actor_active =
      handleLabel(actors[wolf::actor::ventilation_svas4], ui->actor_1);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_1_svas4], ui->actor_2);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_2_svas4], ui->actor_3);
  actor_active |=
      handleLabel(actors[wolf::actor::window_drive_3_svas4], ui->actor_4);
  if (!actor_active) setBlack(ui->controllerName);
}

void MoldForm::setGreen(QLabel *label) {
  ui->controllerName->setStyleSheet("QLabel { color : #66cc00; }");
  label->setStyleSheet("QLabel { color : #66cc00; }");
}

void MoldForm::setBlack(QLabel *label) {
  label->setStyleSheet("QLabel { color : #000000; }");
}

bool MoldForm::handleLabel(
    const wolf::controller::controller::optional_actor &actor, QLabel *label) {
  if (!actor) {
    label->setVisible(false);
    return false;
  } else if (actor.get().state) {
    label->setVisible(true);
    setGreen(label);
    return true;
  } else {
    label->setVisible(true);
    setBlack(label);
    return false;
  }
}

void MoldForm::handleWindow() {
  const auto room = m_dataExchange.get_room(m_room_id);
  if (!room.has_value()) return;
  const auto sensors = room->sensors;
  const auto found = std::find_if(
      sensors.cbegin(), sensors.cend(), [](const wolf::sensor_id &to_check) {
        return wolf::sensor_value_type_helper::is_magnet_switch(to_check);
      });
  if (found == sensors.cend()) return;
  const auto value = m_services.value_handler->get_value_float(*found);
  if (!value.has_value()) return;
  const bool closed = wolf::math::compare_floats(value.get(), 1.f);
  ui->windowClosed->setVisible(closed);
  ui->windowOpen->setVisible(!closed);
}
