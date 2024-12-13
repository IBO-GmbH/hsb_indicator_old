#ifndef MOLD_GUI_ROOMSFORM_HPP
#define MOLD_GUI_ROOMSFORM_HPP

#include "mold/gui/DataExchange.hpp"
#include "mold/gui/WolfServices.hpp"

#include <QPushButton>
#include <QWidget>

namespace Ui {
class RoomsForm;
}

namespace mold {
class configuration_handler;
class DataExchange;
}  // namespace mold

namespace mold {

struct CustomInformationButton : public QPushButton {
  explicit CustomInformationButton(const wolf::types::uuid_array& room_id_,
                                   QWidget* parent = nullptr);
  wolf::types::uuid_array room_id;
};

class RoomsForm : public QWidget {
  Q_OBJECT

 public:
  struct roomIndexes {
    int start;
    int end;
  };

  explicit RoomsForm(const DataExchange& dataExchange, const bool& showAdd,
                     const int page, const int max_page,
                     QWidget* parent = nullptr);
  ~RoomsForm();

 private slots:
  void buildRoomList();

 signals:
  void back();
  void openRoom(wolf::types::uuid_array configuration_id);
  void openNewRoom();
  void pageChanged(const int new_page);

 private slots:
  void handleNextPage();
  void handlePreviousPage();

 private:
  void handleOpenRoom();
  void handleButtonAdd(const wolf::room& room, const int columns,
                       const int index);
  int getRoomCountForPage(const int room_count);
  roomIndexes getRoomIndexes(const int room_count_for_page);
  bool showAdd(const std::size_t& rooms);
  void disablePageButtons();

 private:
  logging::logger m_logger;
  std::unique_ptr<Ui::RoomsForm> ui;

  const DataExchange& m_dataExchange;
  const bool m_showAdd;
  int m_buttonHeight;
  int m_maxButtonWidth;
  int m_page;
  int m_maxPages;

  const int m_maxRoomsPerPage =
      m_dataExchange.getServices().config->room_.max_count_per_page;
};

}  // namespace mold

#endif  // MOLD_GUI_ROOMSFORM_HPP
