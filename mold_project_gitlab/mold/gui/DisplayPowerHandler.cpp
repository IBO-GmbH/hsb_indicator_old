#include "DisplayPowerHandler.hpp"

#include <QEvent>
#include <QProcess>
#include "mold/configuration_state.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/program_options_parser.hpp"

using namespace logging;
using namespace mold;

DisplayPowerHandler::DisplayPowerHandler(DataExchange *const dataExchange,
                                         const WolfServices &services,
                                         const Co2StateHandler &co2_state,
                                         QObject *parent)
    : QObject(parent),
      m_logger{"DisplayPowerHandler"},
      m_dataExchange(dataExchange),
      m_services(services) {
  connect(m_dataExchange, &DataExchange::configuration_states_changed, this,
          &DisplayPowerHandler::handleStatesUpdate);
  connect(m_dataExchange, &DataExchange::display_on_changed, this,
          &DisplayPowerHandler::handleOnChanged);
  connect(&co2_state, &Co2StateHandler::stateOfRoomChanged, this,
          &DisplayPowerHandler::handleCo2StateChanged);
}

void DisplayPowerHandler::setOn(const bool on) {
  m_services.service->post([this, handler = m_services.display_handler, on]() {
    handleOn(handler, on);
  });
}

void DisplayPowerHandler::handleOn(display_handler *handler, const bool on) {
  handler->set_on(on);
}

bool DisplayPowerHandler::eventFilter(QObject *obj, QEvent *event) {
  const bool input = isInput(event);
  if (input) {
    signal_there_was_input();
    const bool wasOff{!m_on};
    // don't proceed if user didnt see anything
    setOn(true);
    if (wasOff) {
      return true;
    }
  }
  return QObject::eventFilter(obj, event);
}

bool DisplayPowerHandler::isInput(QEvent *event) {
  switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:

    case QEvent::TouchBegin:
    case QEvent::TouchCancel:
    case QEvent::TouchEnd:
    case QEvent::TouchUpdate:
      return true;
    default:
      break;
  }
  return false;
}

void DisplayPowerHandler::handleStatesUpdate() {
  m_services.service->post([handler = m_services.display_handler]() {
    handler->handle_states_update();
  });
}

void DisplayPowerHandler::handleOnChanged(const bool on) {
  if (m_on != on) m_on = on;
}

void DisplayPowerHandler::handleCo2StateChanged(
    const wolf::types::uuid_array &, const calculation_state &state) {
  m_services.service->post([handler = m_services.display_handler, state]() {
    handler->handle_co2_state_update(state);
  });
}
