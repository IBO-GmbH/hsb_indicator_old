#ifndef MOLD_GUI_MAINWINDOW_HPP
#define MOLD_GUI_MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include "ControllerTypes.hpp"
#include "LimitTypes.hpp"
#include "RoomNameHandler.hpp"
#include "RoomRotationHandler.hpp"
#include "SensorActivityHandler.hpp"
#include "StatusFormBase.hpp"
#include "StatusListHelper.hpp"
#include "mold/gui/DisplayPowerHandler.hpp"
#include "mold/gui/LanguageHandler.hpp"
#include "mold/gui/WolfServices.hpp"
#include "wifi/access_point.hpp"

namespace Ui {
class MainWindow;
}

namespace mold {
class configuration_handler;
class DataExchange;
class GsmInformationReader;
class RoomForm;
}  // namespace mold

namespace wolf {
class esp3_parser;
class profile_handler;
}  // namespace wolf

namespace mold {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  using access_point = wifi::access_point;
  using sensor_values = std::vector<wolf::sensor_value>;

  explicit MainWindow(const RoomNameHandler &room_name_handler,
                      const RoomRotationHandler &room_rotation_handler,
                      const Co2StateHandler &co2_state_handler,
                      SensorActivityHandler &sensor_activity_handler,
                      QWidget *parent = nullptr);
  ~MainWindow();

  void initialise(WolfServices &services, DataExchange &dataExchange,
                  DisplayPowerHandler *powerHandler, LanguageHandler *language);

 protected slots:
  void openSettings();
  void openStatus();
  void openStatusPage(const StatusListHelper::Status &currentStatus);
  void openNextStatus();
  void openPreviousStatus();
  void openNextStatusReleative(int next);
  void openLanguage();
  void openRooms();
  void openRoom(const wolf::types::uuid_array &room_id);
  void openRoom(const wolf::room &room, const configuration &configuration_,
                const bool is_add, const sensor_values &values = {});
  void openNewRoom();
  void openOptions();
  void openDisplayOptions();
  void openFrsiOptions();
  void openResetOptions();
  void openOptionsPerRoomList();
  void openOptionsPerRoom(const wolf::types::uuid_array &room_id);
  void openOutdoor();
  void openChangeSensor(const wolf::room &room,
                        const configuration &configuration_);
  void openChangeControl(const wolf::room &room);
  void openChangeSensorOffset(const wolf::room &room,
                              const configuration &configuration_);
  void openChangeSensorOffsetOutdoor();
  void openForeignSensorOutdoor();
  void openChangeSVAS(const wolf::room &room,
                      const ControllerTypes &controller_);
  void openChangeActors(const wolf::room &room,
                        const ControllerTypes &controller_);
  void openActorList(const wolf::room &room, const ControllerTypes &controller_,
                     const int button, const int page);
  void openLearnActor(const wolf::room &room,
                      const ControllerTypes &controller_, const int button,
                      const int actor_id);
  void openChangeLimits(const wolf::room &room,
                        const ControllerTypes &controller_,
                        const LimitTypes &type);
  void openDebugStatusScreen();
  void openWifiScreen(const access_point &to_connect,
                      const std::string password);
  void openWifiConnect(const access_point &to_connect);
  void openSensorActivityLog();
  void openPinProtection(const bool change_pin);
  void jumpToWarningScreen(const wolf::types::uuid_array &id,
                           const bool is_co2);
  void openComfortQuestionnaireOptions();
  void openComfortQuestionnaire();
  void openHeatingValveOptions(const wolf::room &room,
                               const configuration &configuration_);
  void openLearnHeatingValve(const wolf::room &room,
                             const configuration &configuration_,
                             const heating_valve_settings &settings);

 private:
  static bool replaceFirstSensor(std::vector<wolf::sensor_id> &container,
                                 const wolf::sensor_id &search,
                                 const wolf::sensor_id &replacement);
  void setMainForm(QWidget *widget);
  boost::optional<std::pair<wolf::room, configuration>> getRoomAndConfiguration(
      const wolf::types::uuid_array &room_id);
  void handleLimitsSVAS1(const wolf::room &room,
                         const ControllerTypes &controller_,
                         const LimitTypes &type);
  void handleNextLimitsSVAS2(const wolf::room &room,
                             const ControllerTypes &controller_,
                             const LimitTypes &type);
  void handlePreviousLimitsSVAS2(const wolf::room &room,
                                 const ControllerTypes &controller_,
                                 const LimitTypes &type);
  void handleNextLimitsSVAS3(const wolf::room &room,
                             const ControllerTypes &controller_,
                             const LimitTypes &type);
  void handlePreviousLimitsSVAS3(const wolf::room &room,
                                 const ControllerTypes &controller_,
                                 const LimitTypes &type);
  void handleNextLimitsSVAS4(const wolf::room &room,
                             const ControllerTypes &controller_,
                             const LimitTypes &type);
  void handlePreviousLimitsSVAS4(const wolf::room &room,
                                 const ControllerTypes &controller_,
                                 const LimitTypes &type);
  void handleNextLimits(const wolf::room &room,
                        const ControllerTypes &controller_,
                        const LimitTypes &type);
  void handlePreviousLimits(const wolf::room &room,
                            const ControllerTypes &controller_,
                            const LimitTypes &type);
  access_point make_invalid_access_point();
  using AsyncDisableAndSaveRoomFormOnComplete =
      std::function<void(const wolf::room &, const configuration &config)>;
  void asyncDisableAndSaveRoomForm(
      RoomForm *const room,
      const AsyncDisableAndSaveRoomFormOnComplete &on_complete);
  void asyncDisableAndSaveRoomFormAndValues(
      RoomForm *const room,
      const AsyncDisableAndSaveRoomFormOnComplete &on_complete,
      const sensor_values &values);
  void changeToLastPage();
  void calculateMaxPage();
  StatusFormBase *getStatus(const StatusListHelper::Status &currentStatus);
  using sensor_ids = std::vector<wolf::sensor_id>;
  static void removeValvesFromSensors(sensor_ids &sensors);
  std::pair<wolf::room, configuration> updateRoom(
      const wolf::room &room, const configuration &configuration_,
      const std::vector<wolf::sensor_profile> &profiles,
      const std::vector<wolf::sensor_value> &values,
      const sensor_ids &sensors) const;
  void handleEnoceanProfilesRoom(const wolf::types::id_esp3 &enocean_sensor_id,
                                 const std::array<std::uint8_t, 3> &eep,
                                 const sensor_ids &learned_ids,
                                 std::vector<wolf::sensor_profile> &profiles,
                                 sensor_ids &new_ids) const;

 private:
  std::unique_ptr<Ui::MainWindow> ui;

  mutable logging::logger m_logger;

  WolfServices *m_services{nullptr};
  DataExchange *m_dataExchange{nullptr};
  DisplayPowerHandler *m_powerHandler{nullptr};
  LanguageHandler *m_language{nullptr};
  GsmInformationReader *m_informationReader{nullptr};

  int m_statusIndex{0};

  int m_roomPage{0};
  int m_maxRoomPage{0};

  QWidget *m_current;

  QTimer m_displayOnOff;

  const RoomNameHandler &m_roomNameHandler;
  const RoomRotationHandler &m_roomRotationHandler;
  const Co2StateHandler &m_co2_state_handler;

  SensorActivityHandler &m_sensor_activity_handler;

  boost::signals2::scoped_connection m_connection_open_questionnaire;
  boost::signals2::scoped_connection m_connection_discard_questionnaire;
};
}  // namespace mold

#endif  // MOLD_GUI_MAINWINDOW_HPP
