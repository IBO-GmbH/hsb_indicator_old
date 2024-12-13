#include "LearnActorForm.hpp"
#include "ui_LearnActorForm.h"

#include "ControllerLabelHandler.hpp"
#include "WolfServices.hpp"
#include "mold/configuration_handler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "wolf/actor/handler.hpp"
#include "wolf/esp3_sender.hpp"

#include <boost/asio/io_service.hpp>

using namespace logging;
using namespace mold;

LearnActorForm::LearnActorForm(WolfServices &services,
                               const ControllerTypes &type,
                               const wolf::types::uuid_array &room_id,
                               const int button, const int actor_id,
                               QWidget *parent)
    : QWidget(parent),
      m_logger{"LearnActorForm"},
      ui(new Ui::LearnActorForm),
      m_services(services),
      m_type(type),
      m_room_id(room_id),
      m_button(button),
      m_base_id(m_services.actor_handler->get_base_id()),
      m_actor_id(static_cast<unsigned int>(actor_id)),
      m_esp3_id(m_base_id + m_actor_id),
      m_learned(false) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &LearnActorForm::back);
  connect(ui->learn, &QPushButton::clicked, this, &LearnActorForm::handleSend);
  connect(ui->on, &QPushButton::clicked, this, &LearnActorForm::handleOn);
  connect(ui->off, &QPushButton::clicked, this, &LearnActorForm::handleOff);

  ControllerLabelHandler::changeLabelActorList(m_type, m_button,
                                               ui->settingLabel);
}

bool LearnActorForm::isLearned() { return m_learned; }

LearnActorForm::~LearnActorForm() = default;

void LearnActorForm::handleSend() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "base_id:" << std::hex << m_base_id << " actor_id:" << m_actor_id;
  auto index = calculateIndex();
  handlePost(m_esp3_id, m_actor_id, index);
  m_learned = true;
}

void LearnActorForm::handleOn() { handleSendOnOff(0x10); }

void LearnActorForm::handleOff() { handleSendOnOff(0x30); }

int LearnActorForm::calculateIndex() {
  auto type_as_int = static_cast<int>(m_type);
  auto index = type_as_int * actors_per_control_loop + m_button;
  return index;
}

void LearnActorForm::sendLearn(wolf::types::id_esp3 id) {
  if (!m_services.esp3_writer) return;
  auto writer = m_services.esp3_writer;
  writer->send_rps(0x10, id, 0x30);
  writer->send_rps(0x00, id, 0x20);
}

void LearnActorForm::handlePost(const wolf::types::id_esp3 &esp3_id,
                                const wolf::types::id_esp3 &actor_id,
                                const int index) {
  setEnabled(false);
  m_services.service->post([controller_handler = m_services.controller_handler,
                            room_id = m_room_id, actor_id, index, this,
                            esp3_id]() {
    sendLearn(esp3_id);
    controller_handler->add_actor(room_id, {actor_id, false},
                                  static_cast<unsigned int>(index));
  });
  setEnabled(true);
}

void LearnActorForm::handleSendOnOff(const uint8_t to_send) {
  if (!m_services.esp3_writer) return;
  auto writer = m_services.esp3_writer;
  writer->send_rps(to_send, m_esp3_id, 0x30);
  writer->send_rps(0x00, m_esp3_id, 0x20);
}
