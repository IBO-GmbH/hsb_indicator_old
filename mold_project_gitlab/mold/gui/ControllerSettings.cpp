#include "ControllerSettings.hpp"
#include "ControllerLabelHandler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "ui_ControllerSettings.h"
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

ControllerSettings::ControllerSettings(WolfServices &services,
                                       DataExchange &dataExchange,
                                       const ControllerTypes &type,
                                       const wolf::types::uuid_array &room_id,
                                       QWidget *parent)
    : QWidget(parent),
      m_logger("ControllerSettings"),
      ui(new Ui::ControllerSettings),
      m_services(services),
      m_dataExchange(dataExchange),
      m_type(type),
      m_room_id(room_id),
      m_load_toggle(false) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &ControllerSettings::back);
  connect(ui->chooseActors, &QPushButton::clicked, this,
          &ControllerSettings::changeActors);
  connect(ui->setLimits, &QPushButton::clicked, this,
          &ControllerSettings::setLimits);
  connect(ui->activateController, &QCheckBox::toggled, this,
          &ControllerSettings::setActiveState);

  try {
    ControllerLabelHandler::changeLabelSettings(m_type, ui->settingLabel);
    loadActiveState();
  } catch (const std::runtime_error &error) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "failed to initialise ControllerSettings. what():" << error.what();
  }
}

ControllerSettings::~ControllerSettings() = default;

ControllerSettings::active_states ControllerSettings::handleType(
    const bool active_state) {
  switch (m_type) {
    case ControllerTypes::sVAS1:
      return {active_state, false, false, false};
    case ControllerTypes::sVAS2:
      return {false, active_state, false, false};
    case ControllerTypes::sVAS3:
      return {false, false, active_state, false};
    case ControllerTypes::sVAS4:
      return {false, false, false, active_state};
  }
  assert(false);
  return {false, false, false, false};
}

void ControllerSettings::setActiveState(const bool checked) {
  if (m_load_toggle) {
    m_load_toggle = false;
    return;
  }
  auto active_states_ = handleType(checked);
  active_states all_false{false, false, false, false};
  m_services.service->post([controller_handler = m_services.controller_handler,
                            room_id = m_room_id, active_states_, all_false]() {
    if (active_states_ != all_false)
      controller_handler->set_active_states(room_id, all_false);
    controller_handler->set_active_states(room_id, active_states_);
  });
}

void ControllerSettings::handleStatesLoaded(const active_states &states) {
  wolf::thread_checker::throw_if_not_ui(
      "ControllerSettings::handleStatesLoaded");
  auto type_as_uint = static_cast<unsigned int>(m_type);
  auto active_state = states[type_as_uint];
  if (active_state) m_load_toggle = true;
  ui->activateController->setChecked(active_state);
}

void ControllerSettings::loadActiveState() {
  auto states = m_dataExchange.get_active_states(m_room_id);
  handleStatesLoaded(states);
}
