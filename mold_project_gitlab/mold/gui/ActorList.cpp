#include "ActorList.hpp"
#include "ControllerLabelHandler.hpp"
#include "ui_ActorList.h"

using namespace mold;

ActorList::ActorList(const ControllerTypes &type, const int button,
                     const int page, QWidget *parent)
    : QWidget(parent), ui(new Ui::ActorList), m_page(page) {
  ui->setupUi(this);
  connect(ui->back, &QPushButton::clicked, this, &ActorList::back);
  connect(ui->actor1, &QPushButton::clicked, this, &ActorList::chooseActor1);
  connect(ui->actor2, &QPushButton::clicked, this, &ActorList::chooseActor2);
  connect(ui->actor3, &QPushButton::clicked, this, &ActorList::chooseActor3);
  connect(ui->actor4, &QPushButton::clicked, this, &ActorList::chooseActor4);
  connect(ui->previous, &QPushButton::clicked, this,
          &ActorList::handlePrevious);
  connect(ui->next, &QPushButton::clicked, this, &ActorList::handleNext);
  changePage(page);

  ControllerLabelHandler::changeLabelActorList(type, button, ui->settingLabel);
}

ActorList::~ActorList() = default;

int ActorList::getPage() const { return m_page; }

void ActorList::changePage(const int page) {
  QString page_label(tr("Page "));
  auto page_as_string = std::to_string(page);
  QString new_page(page_as_string.data());
  page_label += new_page;
  ui->page->setText(page_label);
  changeActorName(page);
}

ActorList::index_array ActorList::getNewActorIds() { return m_new_actor_ids; }

void ActorList::handlePrevious() {
  --m_page;
  if (m_page == 0) m_page = 32;
  emit previous();
}

void ActorList::handleNext() {
  ++m_page;
  const auto max_page = m_max_actors / 4;
  if (m_page > max_page) m_page = 1;
  emit next();
}

void ActorList::changeActorName(const int page) {
  auto new_actors = calculateNewIndex(page);
  auto new_actors_string = convertIndexToString(new_actors);
  auto new_actors_label = makeLabelFromString(new_actors_string);
  setNewActorNames(new_actors_label);
}

ActorList::index_array ActorList::calculateNewIndex(const int page) {
  index_array new_actor;
  auto calculation_page = page - 1;  // first page is 1 but needs to have index
                                     // 0
  for (unsigned int index = 0; index < 4; ++index)
    new_actor[index] = calculation_page * 4 + static_cast<int>(index);
  m_new_actor_ids = new_actor;
  return new_actor;
}

ActorList::string_array ActorList::convertIndexToString(
    const ActorList::index_array &index) {
  string_array index_string;
  for (unsigned int count = 0; count < 4; ++count)
    index_string[count] = std::to_string(index[count]);
  return index_string;
}

ActorList::label_array ActorList::makeLabelFromString(
    const ActorList::string_array &index_string) {
  label_array index_label;
  std::for_each(index_label.begin(), index_label.end(),
                [](QString &label) { label = tr("Actor "); });
  for (unsigned int count = 0; count < 4; ++count)
    index_label[count] += index_string[count].data();
  return index_label;
}

void ActorList::setNewActorNames(const ActorList::label_array &new_names) {
  ui->actor1->setText(new_names[0]);
  ui->actor2->setText(new_names[1]);
  ui->actor3->setText(new_names[2]);
  ui->actor4->setText(new_names[3]);
}
