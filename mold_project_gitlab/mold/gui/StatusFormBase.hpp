#ifndef MOLD_GUI_STATUSFORMBASE_HPP
#define MOLD_GUI_STATUSFORMBASE_HPP

#include <QWidget>
#include "mold/gui/RoomHeaderForm.hpp"
#include "mold/gui/StatusBarForm.hpp"

namespace mold {

namespace statusForm {

struct constructorItems {
  const WolfServices &services;
  const DataExchange &dataExchange;
  DisplayPowerHandler *powerHandler;
  const GsmInformationReader &gsmInformationReader;
  const wolf::types::uuid_array &room_id;
  const RoomRotationHandler &rotation;
};
}  // namespace statusForm

class StatusFormBase : public QWidget {
  Q_OBJECT
 public:
  explicit StatusFormBase(QWidget *parent = nullptr);
  virtual ~StatusFormBase() = default;
  virtual StatusBarForm *getStatusBar() const = 0;
  virtual RoomHeaderForm *getRoomHeader() const = 0;

 signals:
  void openDifferentPage();
};
}  // namespace mold

#endif  // MOLD_GUI_STATUSFORMBASE_HPP
