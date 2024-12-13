#ifndef MOLD_GUI_LEARNACTORFORM_HPP
#define MOLD_GUI_LEARNACTORFORM_HPP

#include <QWidget>
#include <memory>
#include "ControllerTypes.hpp"
#include "log/logger.hpp"
#include "mold/configuration.hpp"
#include "wolf/controller/controller.hpp"
#include "wolf/room.hpp"

namespace Ui {
class LearnActorForm;
}

namespace mold {
class DataExchange;
struct WolfServices;
}  // namespace mold

namespace mold {

class LearnActorForm : public QWidget {
  Q_OBJECT

 public:
  explicit LearnActorForm(WolfServices& services, const ControllerTypes& type,
                          const wolf::types::uuid_array& room_id,
                          const int button, const int actor_id,
                          QWidget* parent = nullptr);
  bool isLearned();
  ~LearnActorForm();

 signals:
  void back();

 private slots:
  void handleSend();
  void handleOn();
  void handleOff();

 private:
  int calculateIndex();
  void sendLearn(wolf::types::id_esp3 id);
  void handlePost(const wolf::types::id_esp3& id,
                  const wolf::types::id_esp3& actor_id, const int index);
  void handleSendOnOff(const std::uint8_t to_send);

 private:
  static constexpr int actors_per_control_loop =
      wolf::controller::controller::actors_per_control_loop;

  logging::logger m_logger;

  std::unique_ptr<Ui::LearnActorForm> ui;

  WolfServices& m_services;

  ControllerTypes m_type;
  wolf::types::uuid_array m_room_id;
  const int m_button;
  const wolf::types::id_esp3 m_base_id;
  const wolf::types::id_esp3 m_actor_id;
  const wolf::types::id_esp3 m_esp3_id;
  bool m_learned;
};
}  // namespace mold

#endif  // MOLD_GUI_LEARNACTORFORM_HPP
