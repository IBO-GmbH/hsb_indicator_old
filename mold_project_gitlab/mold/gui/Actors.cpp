#include "Actors.hpp"
#include "ControllerLabelHandler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "ui_Actors.h"
#include "wolf/actor/actor_indexes.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

Actors::Actors(WolfServices &services, DataExchange &dataExchange,
               const ControllerTypes &type,
               const wolf::types::uuid_array &room_id, QWidget *parent)
    : QWidget(parent),
      m_logger("Actors"),
      ui(new Ui::Actors),
      m_services(services),
      m_dataExchange(dataExchange),
      m_type(type),
      m_room_id(room_id) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this, &Actors::back);
  connect(ui->addActor1, &QPushButton::clicked, this, &Actors::openActorList1);
  connect(ui->addActor2, &QPushButton::clicked, this, &Actors::openActorList2);
  connect(ui->addActor3, &QPushButton::clicked, this, &Actors::openActorList3);
  connect(ui->addActor4, &QPushButton::clicked, this, &Actors::openActorList4);
  connect(ui->removeActors, &QPushButton::clicked, this,
          &Actors::removeAllActors);

  ControllerLabelHandler::changeLabelActors(m_type, ui->settingLabel);
  loadActors();
}

Actors::~Actors() = default;

void Actors::changeActorDescription(const QString &new_description) {
  ui->actorDescription2->setText(new_description);
  ui->actorDescription3->setText(tr("Window Drive 1:"));
  ui->actorDescription4->setText(tr("Window Drive 2:"));
}

void Actors::removeAllActors() {
  if (!checkIfActorsSet()) return;
  ui->addActor1->setText("+");
  ui->addActor2->setText("+");
  ui->addActor3->setText("+");
  ui->addActor4->setText("+");
  handleRemove();
}

QString Actors::convertActorIdToQString(const unsigned int actor_id) {
  QString id_as_qstring = tr("Actor ") + QString::number(actor_id);
  return id_as_qstring;
}

void Actors::setTextButton1(const unsigned int actor_id) {
  QString id_as_qstring = convertActorIdToQString(actor_id);
  ui->addActor1->setText(id_as_qstring);
}

void Actors::setTextButton2(const unsigned int actor_id) {
  QString id_as_qstring = convertActorIdToQString(actor_id);
  ui->addActor2->setText(id_as_qstring);
}

void Actors::setTextButton3(const unsigned int actor_id) {
  QString id_as_qstring = convertActorIdToQString(actor_id);
  ui->addActor3->setText(id_as_qstring);
}

void Actors::setTextButton4(const unsigned int actor_id) {
  QString id_as_qstring = convertActorIdToQString(actor_id);
  ui->addActor4->setText(id_as_qstring);
}

void Actors::remove(const wolf::actor::indexes &indexes) {
  m_services.service->post(
      [controller_handler = m_services.controller_handler, room_id = m_room_id,
       indexes]() { controller_handler->remove_actors(room_id, indexes); });
}

void Actors::handleRemove() {
  switch (m_type) {
    case mold::ControllerTypes::sVAS1:
      remove(
          {wolf::actor::ventilation_svas1, wolf::actor::window_drive_3_svas1});
      break;
    case mold::ControllerTypes::sVAS2:
      remove(
          {wolf::actor::ventilation_svas2, wolf::actor::window_drive_2_svas2});
      break;
    case mold::ControllerTypes::sVAS3:
      remove(
          {wolf::actor::ventilation_svas3, wolf::actor::window_drive_2_svas3});
      break;
    case mold::ControllerTypes::sVAS4:
      remove(
          {wolf::actor::ventilation_svas4, wolf::actor::window_drive_3_svas4});
      break;
  }
}

void Actors::handleTextButton1(const optional_actor &actor) {
  if (!actor) {
    ui->addActor1->setText("+");
    return;
  }
  setTextButton1(actor.get().id);
}

void Actors::handleTextButton2(const Actors::optional_actor &actor) {
  if (!actor) {
    ui->addActor2->setText("+");
    return;
  }
  setTextButton2(actor.get().id);
}

void Actors::handleTextButton3(const Actors::optional_actor &actor) {
  if (!actor) {
    ui->addActor3->setText("+");
    return;
  }
  setTextButton3(actor.get().id);
}

void Actors::handleTextButton4(const Actors::optional_actor &actor) {
  if (!actor) {
    ui->addActor4->setText("+");
    return;
  }
  setTextButton4(actor.get().id);
}

void Actors::handleType(const actor_array &actors) {
  wolf::thread_checker::throw_if_not_ui("Actors::handleType");
  switch (m_type) {
    case mold::ControllerTypes::sVAS1:
      handleTextButton1(actors[wolf::actor::ventilation_svas1]);
      handleTextButton2(actors[wolf::actor::window_drive_1_svas1]);
      handleTextButton3(actors[wolf::actor::window_drive_2_svas1]);
      handleTextButton4(actors[wolf::actor::window_drive_3_svas1]);
      break;
    case mold::ControllerTypes::sVAS2:
      handleTextButton1(actors[wolf::actor::ventilation_svas2]);
      handleTextButton2(actors[wolf::actor::dehumidifier_svas2]);
      handleTextButton3(actors[wolf::actor::window_drive_1_svas2]);
      handleTextButton4(actors[wolf::actor::window_drive_2_svas2]);
      break;
    case mold::ControllerTypes::sVAS3:
      handleTextButton1(actors[wolf::actor::ventilation_svas3]);
      handleTextButton2(actors[wolf::actor::heating_svas3]);
      handleTextButton3(actors[wolf::actor::window_drive_1_svas3]);
      handleTextButton4(actors[wolf::actor::window_drive_2_svas3]);
      break;
    case mold::ControllerTypes::sVAS4:
      handleTextButton1(actors[wolf::actor::ventilation_svas4]);
      handleTextButton2(actors[wolf::actor::window_drive_1_svas4]);
      handleTextButton3(actors[wolf::actor::window_drive_2_svas4]);
      handleTextButton4(actors[wolf::actor::window_drive_3_svas4]);
      break;
  }
}

void Actors::loadActors() {
  auto actors = m_dataExchange.get_actors(m_room_id);
  handleType(actors);
}

bool Actors::checkIfActorsSet() const {
  const std::vector<QPushButton *> buttons = {ui->addActor1, ui->addActor2,
                                              ui->addActor3, ui->addActor4};
  for (const auto &button : buttons)
    if (checkOneActorSet(*button)) return true;
  return false;
}

bool Actors::checkOneActorSet(const QPushButton &actor) {
  return actor.text() != "+";
}
