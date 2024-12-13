#ifndef MOLD_GUI_ACTORS_HPP
#define MOLD_GUI_ACTORS_HPP

#include <QPushButton>
#include <QWidget>
#include <memory>
#include "log/logger.hpp"
#include "mold/gui/ControllerTypes.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/WolfServices.hpp"

namespace Ui {
class Actors;
}

namespace mold {

class Actors : public QWidget {
  Q_OBJECT

 public:
  using actor_array = wolf::controller::controller::actor_array;
  using optional_actor = wolf::controller::controller::optional_actor;

  explicit Actors(WolfServices &services, DataExchange &dataExchange,
                  const ControllerTypes &type,
                  const wolf::types::uuid_array &room_id,
                  QWidget *parent = nullptr);
  ~Actors();
  void changeActorDescription(const QString &new_description);
  void removeAllActors();

 private:
  QString convertActorIdToQString(const unsigned int actor_id);
  void setTextButton1(const unsigned int actor_id);
  void setTextButton2(const unsigned int actor_id);
  void setTextButton3(const unsigned int actor_id);
  void setTextButton4(const unsigned int actor_id);
  void remove(const wolf::actor::indexes &indexes);
  void handleRemove();
  void handleType(const actor_array &actors);
  void handleTextButton1(const optional_actor &actor);
  void handleTextButton2(const optional_actor &actor);
  void handleTextButton3(const optional_actor &actor);
  void handleTextButton4(const optional_actor &actor);
  void loadActors();
  bool checkIfActorsSet() const;
  static bool checkOneActorSet(const QPushButton &actor);

 signals:
  void back();
  void openActorList1();
  void openActorList2();
  void openActorList3();
  void openActorList4();

 private:
  logging::logger m_logger;

  std::unique_ptr<Ui::Actors> ui;

  WolfServices &m_services;
  DataExchange &m_dataExchange;

  ControllerTypes m_type;
  wolf::types::uuid_array m_room_id;
};
}  // namespace mold
#endif  // MOLD_GUI_ACTORS_HPP
