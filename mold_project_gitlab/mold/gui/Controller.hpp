#ifndef MOLD_MOLD_GUI_CONTROLLER_HPP
#define MOLD_MOLD_GUI_CONTROLLER_HPP

#include <QWidget>
#include <memory>

#include "wolf/room.hpp"

namespace Ui {
class Controller;
}

namespace mold {

class Controller : public QWidget {
  Q_OBJECT

 public:
  explicit Controller(const wolf::room& room, QWidget* parent = nullptr);
  ~Controller();

  const wolf::room& getRoom() const;

 signals:
  void back();
  void changeSVAS1();
  void changeSVAS2();
  void changeSVAS3();
  void changeSVAS4();

 private:
  std::unique_ptr<Ui::Controller> ui;

  const wolf::room m_room;
};
}
#endif  // MOLD_MOLD_GUI_CONTROLLER_HPP
