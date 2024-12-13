#ifndef MOLD_GUI_MOLDFORM_HPP
#define MOLD_GUI_MOLDFORM_HPP

#include <QWidget>
#include "ControllerTypes.hpp"
#include "mold/configuration_state.hpp"
#include "mold/gui/DataExchange.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/RoomHeaderForm.hpp"
#include "mold/gui/StatusBarForm.hpp"
#include "mold/gui/StatusFormBase.hpp"
#include "mold/gui/WolfServices.hpp"

class QLabel;

namespace Ui {
class MoldForm;
}

namespace mold {
class MoldValuesForm;

class MoldForm : public StatusFormBase {
  Q_OBJECT

 public:
  using duration = std::chrono::system_clock::duration;

  explicit MoldForm(statusForm::constructorItems& items,
                    QWidget* parent = nullptr);
  ~MoldForm() override;

  StatusBarForm* getStatusBar() const override;
  RoomHeaderForm* getRoomHeader() const override;

 private slots:
  void updateOutputWithCatch();
  void updateOutput();

 private:
  using optionalType = boost::optional<ControllerTypes>;
  void updateStateUI(const bool did_calculation_state_change);
  void setWarningMessage(const QString& head, const QString& text);
  void updateControllerView();
  bool checkIfActive(optionalType& type);
  optionalType setControllerType(
      const wolf::controller::values_per_room::active_states& active_states);
  void setActors(const ControllerTypes& type);
  void setActorsVentilation();
  void setActorsDehumidifier();
  void setActorsHeating();
  void setActorsVentilation2();
  void handleActors(const ControllerTypes& type);
  void handleActorsSVAS1(
      const wolf::controller::controller::actor_array& actors);
  void handleActorsSVAS2(
      const wolf::controller::controller::actor_array& actors);
  void handleActorsSVAS3(
      const wolf::controller::controller::actor_array& actors);
  void handleActorsSVAS4(
      const wolf::controller::controller::actor_array& actors);
  void setGreen(QLabel* label);
  void setBlack(QLabel* label);
  bool handleLabel(const wolf::controller::controller::optional_actor& actor,
                   QLabel* label);
  void handleWindow();

 private:
  std::unique_ptr<Ui::MoldForm> ui;
  MoldValuesForm* m_moldValues{nullptr};

  logging::logger m_logger;

  const WolfServices& m_services;
  const DataExchange& m_dataExchange;

  const wolf::types::uuid_array m_room_id;
  configuration m_configuration;
  configuration_state m_state;

  const duration m_calculation_timeout =
      std::chrono::minutes(m_services.config->mold_states_.calculation_timeout);
};
}  // namespace mold

#endif  // MOLD_GUI_MOLDFORM_HPP
