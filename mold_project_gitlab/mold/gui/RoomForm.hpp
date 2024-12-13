#ifndef MOLD_GUI_ROOMFORM_HPP
#define MOLD_GUI_ROOMFORM_HPP

#include <QWidget>
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/RoomNameHandler.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wolf/room.hpp"

namespace Ui {
class RoomForm;
}

namespace mold {

class RoomForm : public QWidget {
  Q_OBJECT

 public:
  explicit RoomForm(const WolfServices& services,
                    const DataExchange& dataExchange, const wolf::room& room_,
                    const configuration& configuration_,
                    const RoomNameHandler& nameHandler,
                    QWidget* parent = nullptr);
  ~RoomForm();

  const configuration& getConfiguration() const;
  const wolf::room& getRoom() const;

 signals:
  void back();
  void removed();
  void changeControl();
  void changeSensor();
  void changeSensorOffset();

 private:
  void save_name();
  void handleRemove();
  void handleRemoved();
  void handleChangeSensor();
  void handleChangeControl();
  void handleChangeSensorOffset();
  bool hasOnBoardSensors() const;
  void handleRoomUpdate(const wolf::room& room_);

 private:
  std::unique_ptr<Ui::RoomForm> ui;

  mutable logging::logger m_logger;

  const mold::WolfServices& m_services;
  const DataExchange& m_dataExchange;

  wolf::room m_currentRoom;
  configuration m_currentConfiguration;
  QStringList m_nameList;

  boost::signals2::scoped_connection m_connection_learn;
  boost::signals2::scoped_connection m_connection_esp3_data;
  boost::signals2::scoped_connection m_connection_enocean_data;

  const RoomNameHandler& m_nameHandler;
};
}  // namespace mold

#endif  // MOLD_GUI_ROOMFORM_HPP
