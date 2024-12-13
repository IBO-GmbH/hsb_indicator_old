#include "RoomHeaderForm.hpp"
#include "ui_RoomHeaderForm.h"

#include "RoomNameHandler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/program_options_parser.hpp"
#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

RoomHeaderForm::RoomHeaderForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::RoomHeaderForm), m_logger("RoomHeaderForm") {
  ui->setupUi(this);

  connect(ui->next, &QToolButton::clicked, this, &RoomHeaderForm::next);
  connect(ui->previous, &QToolButton::clicked, this, &RoomHeaderForm::previous);
  connect(ui->nextBig, &InvisibleClickableButton::clicked, ui->next,
          &QPushButton::click);
  connect(ui->previousBig, &InvisibleClickableButton::clicked, ui->previous,
          &QPushButton::click);
}

RoomHeaderForm::~RoomHeaderForm() = default;

void RoomHeaderForm::initialise(const wolf::types::uuid_array &room_id,
                                const DataExchange &dataExchange,
                                const WolfServices &services,
                                DisplayPowerHandler *const powerHandler,
                                const RoomRotationHandler &rotation,
                                const bool is_co2) {
  m_room_id = room_id;
  m_dataExchange = &dataExchange;

  connect(m_dataExchange, &DataExchange::room_updated, this,
          &RoomHeaderForm::updateOutput);
  connect(&rotation, &RoomRotationHandler::rotationNotLocked, this,
          [this, is_co2](const auto &id, const bool is_co2_signal) {
            if (id != m_room_id || is_co2 != is_co2_signal) return;
            startRotation();
          });
  connect(&rotation, &RoomRotationHandler::jumpToRoom, this,
          [this, &is_co2](const auto &id, const bool is_co2_signal) {
            if (id == m_room_id && is_co2 == is_co2_signal) return;
            emit jump(id, is_co2_signal);
          });

  updateOutput();
  const bool moreThanOneRoom = m_dataExchange->moreThanOneConfiguration();
  ui->next->setVisible(moreThanOneRoom);
  ui->nextBig->setVisible(moreThanOneRoom);
  ui->previous->setVisible(moreThanOneRoom);
  ui->previousBig->setVisible(moreThanOneRoom);

  auto rotation_enabled = dataExchange.is_rotation_enabled();
  auto rotation_locked = rotation.rotationLockedForRoom(m_room_id, is_co2);
  if (services.config->room_.switch_timeout > 0 && moreThanOneRoom &&
      rotation_enabled) {
    m_timer.setInterval(services.config->room_.switch_timeout *
                        1000);  // *1000 for sec to msec
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &RoomHeaderForm::next);
    if (rotation_locked) return;
    m_timer.start();
    m_connection_there_was_input = powerHandler->signal_there_was_input.connect(
        [this]() { startRotation(); });
  }
}

int RoomHeaderForm::getHeightNameLabel() const {
  return ui->roomName->height();
}

void RoomHeaderForm::stopRotation() {
  wolf::thread_checker::throw_if_not_ui("RoomHeaderForm::stopRotation");
  if (!m_timer.isActive()) return;
  m_timer.stop();
}

void RoomHeaderForm::updateOutput() {
  boost::optional<wolf::room> room_check = m_dataExchange->get_room(m_room_id);
  if (!room_check) {
    LOGGING_LOG(m_logger, severity::warning)
        << "updateOutput(): could not find room! id:" << m_room_id;
    throw std::runtime_error("RoomHeaderForm::updateOutput: room not found");
  }
  if (m_room != room_check.get()) {
    m_room = room_check.get();
    updateRoomUI();
  }
}

void RoomHeaderForm::updateRoomUI() {
  LOGGING_LOG(m_logger, severity::verbose) << "updateRoomUI(), room:" << m_room;
  const auto width = ui->roomName->width() - 40;
  const QString croppedText = RoomNameHandler::getCroppedRoomName(
      ui->roomName->font(), width, QString::fromStdString(m_room.name));
  ui->roomName->setText(croppedText);
}

void RoomHeaderForm::startRotation() {
  wolf::thread_checker::throw_if_not_ui("RoomHeaderForm::startRotation");
  // restart rotate
  LOGGING_LOG(m_logger, severity::verbose) << "start rotation";
  if (m_timer.isActive()) return;
  m_timer.start();
}
