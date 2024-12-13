#include "RoomsForm.hpp"
#include "ui_RoomsForm.h"

#include "RoomNameHandler.hpp"
#include "mold/program_options_parser.hpp"
#include "wolf/room.hpp"

using namespace mold;

mold::CustomInformationButton::CustomInformationButton(
    const wolf::types::uuid_array& room_id_, QWidget* parent)
    : QPushButton(parent), room_id(room_id_) {}

RoomsForm::RoomsForm(const DataExchange& dataExchange, const bool& showAdd,
                     const int page, const int max_page, QWidget* parent)
    : QWidget(parent),
      m_logger("RoomsForm"),
      ui(new Ui::RoomsForm),
      m_dataExchange(dataExchange),
      m_showAdd{showAdd},
      m_page(page),
      m_maxPages(max_page) {
  ui->setupUi(this);

  const auto maxRoomsHeight = m_maxRoomsPerPage / 2;

  const auto pixelForSpacing = 24;
  const auto spacing = pixelForSpacing / (maxRoomsHeight - 1);
  ui->rooms->layout()->setSpacing(spacing);

  const std::pair<int, int> containerSizes = [this] {
    int marginBottom;
    int marginTop;
    int marginLeft;
    int marginRight;
    ui->roomsContainer->layout()->getContentsMargins(
        &marginLeft, &marginTop, &marginRight, &marginBottom);
    return std::make_pair<int, int>(
        ui->roomsContainer->width() - (marginLeft + marginRight),
        ui->roomsContainer->height() - (marginBottom + marginTop));
  }();
  const auto heightLeft = containerSizes.second - pixelForSpacing;

  m_buttonHeight = heightLeft / maxRoomsHeight;
  m_maxButtonWidth = containerSizes.first - pixelForSpacing;

  connect(ui->back, &QPushButton::clicked, this, &RoomsForm::back);
  connect(ui->addRoom, &QPushButton::clicked, this, &RoomsForm::openNewRoom);
  connect(ui->nextPage, &QPushButton::clicked, this,
          &RoomsForm::handleNextPage);
  connect(ui->previousPage, &QPushButton::clicked, this,
          &RoomsForm::handlePreviousPage);
  connect(&m_dataExchange, &DataExchange::room_added, this,
          &RoomsForm::buildRoomList);
  connect(&m_dataExchange, &DataExchange::room_updated, this,
          &RoomsForm::buildRoomList);
  connect(&m_dataExchange, &DataExchange::room_removed, this,
          &RoomsForm::buildRoomList);

  if (m_maxPages == 0) disablePageButtons();

  buildRoomList();
}

RoomsForm::~RoomsForm() = default;

void RoomsForm::buildRoomList() {
  while (!ui->gridLayout->isEmpty()) {
    auto delete_me = ui->gridLayout->takeAt(0);
    delete delete_me->widget();
    delete delete_me;
  }

  const auto& rooms = m_dataExchange.get_rooms();
  const int room_count_for_page =
      getRoomCountForPage(static_cast<int>(rooms.size()));
  const auto room_indexes = getRoomIndexes(room_count_for_page);
  const int columns = static_cast<int>(std::ceil(
      static_cast<float>(room_count_for_page) / (m_maxRoomsPerPage / 2)));

  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "number of rooms: " << rooms.size()
      << " room_count_for_page:" << room_count_for_page
      << " page number: " << m_page << " max pages: " << m_maxPages
      << " room index start:" << room_indexes.start
      << " end:" << room_indexes.end << " columns:" << columns;

  int index{0};
  for (int count = room_indexes.start; count <= room_indexes.end; ++count) {
    handleButtonAdd(rooms[static_cast<unsigned int>(count)], columns, index);
    ++index;
  }

  if (!showAdd(rooms.size())) {
    ui->addRoom->setEnabled(false);
    return;
  }
}

void RoomsForm::handleNextPage() {
  ++m_page;
  if (m_page > m_maxPages) m_page = 0;
  pageChanged(m_page);
  buildRoomList();
}

void RoomsForm::handlePreviousPage() {
  --m_page;
  if (m_page < 0) m_page = m_maxPages;
  pageChanged(m_page);
  buildRoomList();
}

void RoomsForm::handleOpenRoom() {
  const CustomInformationButton* sender_ =
      static_cast<CustomInformationButton*>(QObject::sender());
  emit openRoom(sender_->room_id);
}

void RoomsForm::handleButtonAdd(const wolf::room& room, const int columns,
                                const int index) {
  CustomInformationButton* result = new CustomInformationButton(room.id, this);
  const int button_width = m_maxButtonWidth / columns;

  const int text_width = button_width - 30;
  const QString text = RoomNameHandler::getCroppedRoomName(
      result->font(), text_width, QString::fromStdString(room.name));
  result->setText(text);
  result->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  result->setMinimumWidth(button_width);
  result->setMaximumHeight(50);
  result->setMinimumHeight(m_buttonHeight);
  const int row = index % (m_maxRoomsPerPage / 2);
  const int column = index / (m_maxRoomsPerPage / 2);
  ui->gridLayout->addWidget(result, row, column);

  connect(result, &QPushButton::clicked, this, &RoomsForm::handleOpenRoom);
}

int RoomsForm::getRoomCountForPage(const int room_count) {
  if (room_count <= 0) return 0;

  if (m_page == m_maxPages) {
    const int room_count_for_page =
        (room_count - m_maxPages * m_maxRoomsPerPage) % m_maxRoomsPerPage;
    if (room_count_for_page > 0) return room_count_for_page;
  }
  return m_maxRoomsPerPage;
}

RoomsForm::roomIndexes RoomsForm::getRoomIndexes(
    const int room_count_for_page) {
  const int room_start = static_cast<int>((m_page * m_maxRoomsPerPage));
  const int room_end = room_start + static_cast<int>(room_count_for_page) - 1;
  return {room_start, room_end};
}

bool RoomsForm::showAdd(const std::size_t& rooms) {
  const auto maxRooms = m_dataExchange.getServices().config->room_.max_count;
  if (maxRooms == 0) return m_showAdd;
  if (rooms >= static_cast<std::size_t>(maxRooms)) return false;
  return true;
}

void RoomsForm::disablePageButtons() {
  ui->nextPage->setDisabled(true);
  ui->previousPage->setDisabled(true);
}
