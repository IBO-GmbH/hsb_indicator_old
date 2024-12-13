#ifndef MOLD_GUI_ROOMHEADERFORM_HPP
#define MOLD_GUI_ROOMHEADERFORM_HPP

#include "log/logger.hpp"

#include <QTimer>
#include <QWidget>
#include <boost/signals2/connection.hpp>
#include "RoomRotationHandler.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wolf/room.hpp"

namespace Ui {
class RoomHeaderForm;
}

namespace mold {

class RoomHeaderForm : public QWidget {
  Q_OBJECT

 public:
  explicit RoomHeaderForm(QWidget *parent = nullptr);
  ~RoomHeaderForm();

  void initialise(const wolf::types::uuid_array &room_id,
                  const DataExchange &dataExchange,
                  const WolfServices &services,
                  DisplayPowerHandler *const powerHandler,
                  const RoomRotationHandler &rotation, const bool is_co2);

  int getHeightNameLabel() const;

 signals:
  void previous();
  void next();
  void jump(const wolf::types::uuid_array &id, const bool is_co2);

 private slots:
  void updateOutput();

 private:
  void updateRoomUI();
  void stopRotation();
  void startRotation();

 private:
  std::unique_ptr<Ui::RoomHeaderForm> ui;

  logging::logger m_logger;

  wolf::types::uuid_array m_room_id;
  const DataExchange *m_dataExchange{nullptr};

  QTimer m_timer;

  wolf::room m_room;

  boost::signals2::scoped_connection
      m_connection_there_was_input;  // from ui to ui thread!
};
}  // namespace mold

#endif  // MOLD_GUI_ROOMHEADERFORM_HPP
