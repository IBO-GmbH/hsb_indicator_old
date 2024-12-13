#include "RoomForm.hpp"
#include <QAbstractItemView>
#include <boost/asio/io_service.hpp>
#include "DataExchange.hpp"
#include "mold/gui/PostToGUIThread.hpp"
#include "mold/program_options_parser.hpp"
#include "ui_RoomForm.h"
#include "wolf/onboard_sensor_checker.hpp"
#include "wolf/room_handler.hpp"

using namespace logging;
using namespace mold;

RoomForm::RoomForm(const WolfServices &services,
                   const DataExchange &dataExchange, const wolf::room &room_,
                   const configuration &configuration_,
                   const RoomNameHandler &nameHandler,

                   QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RoomForm),
      m_logger{"RoomForm"},
      m_services(services),
      m_dataExchange(dataExchange),
      m_currentRoom(room_),
      m_currentConfiguration(configuration_),
      m_nameHandler(nameHandler) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "openeing ui room with room:" << m_currentRoom
      << " configuration:" << m_currentConfiguration;

#ifndef NDEBUG
  if (m_currentRoom.id == wolf::types::uuid_array{})
    throw std::runtime_error("RoomForm::RoomForm: room id is empty array");
  if (m_currentRoom.sensors.empty())
    throw std::runtime_error("RoomForm::RoomForm: no sensors in room");
#endif

  const bool notForeignSensor = [this] {
    if (m_currentConfiguration.temperature.has_owner()) return false;
    if (m_currentConfiguration.humidity.has_owner()) return false;
    return true;
  }();

  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &RoomForm::save_name);
  connect(ui->back, &QPushButton::clicked, this, &RoomForm::back);
  connect(ui->changeControl, &QPushButton::clicked, this,
          &RoomForm::handleChangeControl);
  connect(ui->changeSensor, &QPushButton::clicked, this,
          &RoomForm::handleChangeSensor);
  connect(ui->changeSensorOffset, &QPushButton::clicked, this,
          &RoomForm::handleChangeSensorOffset);
  connect(ui->remove, &QPushButton::clicked, this, &RoomForm::handleRemove);
  connect(&m_dataExchange, &DataExchange::room_updated, this,
          &RoomForm::handleRoomUpdate);

  ui->changeSensorOffset->setVisible(
      m_services.config->general_.use_sensor_offset &&
      m_currentConfiguration.temperature.is_set() && notForeignSensor);
  if (hasOnBoardSensors()) {
    // you can not delete the room with the integrated sensors!
    ui->remove->hide();
    ui->changeSensor->hide();
  }
  m_nameList = m_nameHandler.generateRoomList();
  const QString currentRoomName = QString::fromStdString(m_currentRoom.name);
  m_nameHandler.addIfNewName(m_nameList, currentRoomName);
  const auto width =
      ui->changeName->width() - ui->changeName->iconSize().width() - 20;
  QStringList croppedNames =
      m_nameHandler.cropRoomNameList(m_nameList, ui->changeName->font(), width);
  int index = m_nameHandler.getNameIndex(m_nameList, currentRoomName);
  for (const QString &roomName : croppedNames) {
    ui->changeName->addItem(roomName);
  }
  ui->changeName->setCurrentIndex(index);
  ui->changeName->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

RoomForm::~RoomForm() = default;

const configuration &RoomForm::getConfiguration() const {
  return m_currentConfiguration;
}

const wolf::room &RoomForm::getRoom() const { return m_currentRoom; }

void RoomForm::save_name() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "save_name, room:" << m_currentRoom
      << " configuration:" << m_currentConfiguration;

  auto currentIndex = ui->changeName->currentIndex();
  m_currentRoom.name = m_nameList.at(currentIndex).toStdString();
}

void RoomForm::handleRemove() {
  setEnabled(false);
  m_services.service->post([this, configs = m_services.configurations,
                            id = m_currentConfiguration.id,
                            rooms = m_services.rooms,
                            room_id = m_currentRoom.id]() {
    // TODO id/m_currentConfiguration should be an optional!
    if (id != wolf::types::uuid_array{}) configs->remove(id);
    if (!rooms->remove(room_id)) {
      LOGGING_LOG(m_logger, severity::error) << "could not remove room";
    }
    postToGUIThread([this] { handleRemoved(); });
  });
}

void RoomForm::handleRemoved() { emit removed(); }

void RoomForm::handleChangeSensor() {
  save_name();
  emit changeSensor();
}

void RoomForm::handleChangeControl() {
  save_name();
  emit changeControl();
}

void RoomForm::handleChangeSensorOffset() {
  save_name();
  emit changeSensorOffset();
}

bool RoomForm::hasOnBoardSensors() const {
  return wolf::onboard_sensor_checker::has_onboard_sensor(m_currentRoom);
}

void RoomForm::handleRoomUpdate(const wolf::room &room_) {
  if (m_currentRoom.id != room_.id) return;
  LOGGING_LOG(m_logger, severity::normal)
      << "room changed while on settings page for this room, will use updated "
         "room: "
      << room_;
  m_currentRoom = room_;
  const auto config_for_room =
      m_services.configurations->get_by_room_id(room_.id);
  if (config_for_room.has_value())
    m_currentConfiguration = config_for_room.value();
}
