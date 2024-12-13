#include "MainWindow.hpp"

#include <qprocess.h>

#include <QFile>
#include <QFontDatabase>
#include <QLayout>
#include <QPropertyAnimation>
#include <boost/asio/io_service.hpp>

#include "ActorList.hpp"
#include "Actors.hpp"
#include "ChangeLanguageForm.hpp"
#include "Co2Form.hpp"
#include "ComfortQuestionnaire.hpp"
#include "ComfortQuestionnaireOptions.hpp"
#include "Controller.hpp"
#include "ControllerSettings.hpp"
#include "CurrentClampForm.hpp"
#include "DataExchange.hpp"
#include "DebugStatusScreen.hpp"
#include "DisplayOptions.hpp"
#include "ForeignSensorForm.hpp"
#include "FrsiOptions.hpp"
#include "GenericValueForm.hpp"
#include "GsmInformationReader.hpp"
#include "HeatingValveSettings.hpp"
#include "LearnActorForm.hpp"
#include "LearnHeatingValve.hpp"
#include "LearnSensorForm.hpp"
#include "Limits.hpp"
#include "MoldForm.hpp"
#include "OptionsForm.hpp"
#include "OptionsPerRoomForm.hpp"
#include "OutDoorForm.hpp"
#include "ParticlesForm.hpp"
#include "PinProtectionForm.hpp"
#include "PostToGUIThread.hpp"
#include "ResetOptions.hpp"
#include "RoomForm.hpp"
#include "RoomHeaderForm.hpp"
#include "RoomsForm.hpp"
#include "SensorActivityLog.hpp"
#include "SensorOffsetForm.hpp"
#include "SettingsForm.hpp"
#include "StatusBarForm.hpp"
#include "WifiConnectForm.hpp"
#include "WifiForm.hpp"
#include "mold/heating_valve_helper.hpp"
#include "mold/outdoor_sensor_change_helper.hpp"
#include "mold/room_creator.hpp"
#include "mold/sensor_setter_helper.hpp"
#include "mold/sensor_to_room_and_configuration_helper.hpp"
#include "ui_MainWindow.h"
#include "wolf/math_clamp.hpp"
#include "wolf/profile_handler.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/sensor_id_rssi.hpp"
#include "wolf/sensor_id_scd30.hpp"
#include "wolf/sensor_id_sht25.hpp"
#include "wolf/sensor_id_shtc1.hpp"
#include "wolf/sensor_value_handler.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/uuid_helper.hpp"

using namespace mold;
using namespace logging;

MainWindow::MainWindow(const RoomNameHandler &room_name_handler,
                       const RoomRotationHandler &room_rotation_handler,
                       const Co2StateHandler &co2_state_handler,
                       SensorActivityHandler &sensor_activity_handler,
                       QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_logger{"MainWindow"},
      m_current{nullptr},
      m_roomNameHandler(room_name_handler),
      m_roomRotationHandler(room_rotation_handler),
      m_co2_state_handler(co2_state_handler),
      m_sensor_activity_handler(sensor_activity_handler) {
  ui->setupUi(this);

  m_current = ui->centralwidget;
  connect(&m_roomRotationHandler, &RoomRotationHandler::jumpToRoom, this,
          &MainWindow::jumpToWarningScreen);
}

MainWindow::~MainWindow() {
  LOGGING_LOG(m_logger, severity::verbose) << "MainWindow::~MainWindow()";
  if (m_current) {
    m_current->deleteLater();
  }
}

void MainWindow::initialise(WolfServices &services, DataExchange &dataExchange,
                            DisplayPowerHandler *powerHandler,
                            LanguageHandler *language) {
  m_services = &services;
  m_dataExchange = &dataExchange;
  m_powerHandler = powerHandler;
  m_language = language;
  m_informationReader = new GsmInformationReader(services, this);
  m_connection_open_questionnaire =
      m_services->comfort_questionnaire_handler->signal_open_next_questionnaire
          .connect([this]() {
            LOGGING_LOG(m_logger, severity::verbose)
                << "open new questionnaire";
            postToGUIThread([this] { openComfortQuestionnaire(); });
          });
  m_connection_discard_questionnaire =
      m_services->comfort_questionnaire_handler
          ->signal_cancel_last_questionnaire.connect([this]() {
            LOGGING_LOG(m_logger, severity::verbose)
                << "discard last questionnaire";
            postToGUIThread([this] { openStatus(); });
          });
  openStatus();
}

void MainWindow::openSettings() {
  SettingsForm *settings = new SettingsForm(*m_dataExchange, this);
  connect(settings, &SettingsForm::back, this, &MainWindow::openStatus);
  connect(settings, &SettingsForm::language, this, &MainWindow::openLanguage);
  connect(settings, &SettingsForm::rooms, [this]() {
    m_roomPage = 0;
    openRooms();
  });
  connect(settings, &SettingsForm::outdoor, this, &MainWindow::openOutdoor);
  connect(settings, &SettingsForm::options, this, &MainWindow::openOptions);
  connect(settings, &SettingsForm::wifi, this,
          [this]() { openWifiScreen(make_invalid_access_point(), ""); });
  connect(settings, &SettingsForm::activity, this,
          &MainWindow::openSensorActivityLog);
  connect(settings, &SettingsForm::changePin, this,
          [this]() { openPinProtection(true); });
  setMainForm(settings);
}

void MainWindow::openStatus() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "openStatus, currentStatusIndex:" << m_statusIndex;

  const StatusListHelper helper(*m_dataExchange);
  auto statusList = helper.getStatusList();
  if (statusList.empty()) {
    openNewRoom();
    return;
  }

  if (m_statusIndex >= static_cast<int>(statusList.size())) m_statusIndex = 0;
  if (m_statusIndex < 0)
    m_statusIndex = static_cast<int>(statusList.size()) - 1;

  const auto &currentStatus =
      statusList[static_cast<std::size_t>(m_statusIndex)];
  openStatusPage(currentStatus);
}

void MainWindow::openStatusPage(const StatusListHelper::Status &currentStatus) {
  if (m_informationReader == nullptr)
    throw std::runtime_error(
        "MainWindow::openStatusPage: m_informationReader is a null pointer!");

  StatusFormBase *status = getStatus(currentStatus);
  if (!status)
    throw std::runtime_error(
        "MainWindow::openStatusPage: type of status not handled");

  connect(status->getStatusBar(), &StatusBarForm::openSettings, this,
          [this]() { openPinProtection(false); });
  connect(status->getRoomHeader(), &RoomHeaderForm::next, this,
          &MainWindow::openNextStatus);
  connect(status->getRoomHeader(), &RoomHeaderForm::previous, this,
          &MainWindow::openPreviousStatus);
  connect(status->getRoomHeader(), &RoomHeaderForm::jump, this,
          [this](const auto &id, const bool is_co2) {
            jumpToWarningScreen(id, is_co2);
            openStatus();
          });
  connect(status, &StatusFormBase::openDifferentPage, this,
          [this, currentStatus]() {
            const auto room_and_config =
                getRoomAndConfiguration(currentStatus.room);
            openHeatingValveOptions(room_and_config->first,
                                    room_and_config->second);
          });
  setMainForm(status);
}

void MainWindow::openNextStatus() { openNextStatusReleative(+1); }

void MainWindow::openPreviousStatus() { openNextStatusReleative(-1); }

void MainWindow::openNextStatusReleative(int next) {
  m_statusIndex += next;
  openStatus();
}

void MainWindow::openLanguage() {
  ChangeLanguageForm *language =
      new ChangeLanguageForm(*m_services, m_language, this);
  connect(language, &ChangeLanguageForm::back, this, &MainWindow::openSettings);
  connect(language, &ChangeLanguageForm::debugStatusScreen, this,
          &MainWindow::openDebugStatusScreen);
  setMainForm(language);
}

void MainWindow::openRooms() {
  calculateMaxPage();
  RoomsForm *rooms =
      new RoomsForm(*m_dataExchange, true, m_roomPage, m_maxRoomPage, this);
  connect(rooms, &RoomsForm::back, this, &MainWindow::openSettings);
  connect(rooms, &RoomsForm::openNewRoom, this, &MainWindow::openNewRoom);
  connect(
      rooms, &RoomsForm::openRoom,
      [this](const wolf::types::uuid_array &room_id) { openRoom(room_id); });
  connect(rooms, &RoomsForm::pageChanged,
          [this](const int new_page) { m_roomPage = new_page; });
  setMainForm(rooms);
}

void MainWindow::openRoom(const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "MainWindow::openRoom(), room_id:" << room_id;

  const auto roomAndConfig = getRoomAndConfiguration(room_id);
  if (!roomAndConfig) {
    LOGGING_LOG(m_logger, severity::error) << "failed to open room";
    throw std::runtime_error("MainWindow::openRoom: no room and config!");
  }
  openRoom(roomAndConfig.get().first, roomAndConfig.get().second, false);
}

void MainWindow::asyncDisableAndSaveRoomForm(
    RoomForm *const room,
    const AsyncDisableAndSaveRoomFormOnComplete &on_complete) {
  asyncDisableAndSaveRoomFormAndValues(room, on_complete, {});
}

void MainWindow::asyncDisableAndSaveRoomFormAndValues(
    RoomForm *const room,
    const MainWindow::AsyncDisableAndSaveRoomFormOnComplete &on_complete,
    const MainWindow::sensor_values &values) {
  room->setEnabled(false);
  auto room_config = std::make_pair(room->getRoom(), room->getConfiguration());
  m_services->service->post([this, room_config, on_complete, values]() mutable {
    try {
      room_config = m_services->room_and_configuration_saver->save_and_update(
          room_config.first, room_config.second);
      if (!values.empty()) m_services->value_handler->handle_list(values);
    } catch (const std::runtime_error &error) {
      LOGGING_LOG(m_logger, severity::error)
          << "openRoom, back. could not save room and configuration. what():"
          << error.what();
    }
    postToGUIThread([on_complete, room_config] {
      on_complete(room_config.first, room_config.second);
    });
  });
}

void MainWindow::changeToLastPage() {
  calculateMaxPage();
  m_roomPage = m_maxRoomPage;
}

void MainWindow::calculateMaxPage() {
  const int max_rooms_per_page = m_services->config->room_.max_count_per_page;
  const int room_count = m_dataExchange->get_room_count();
  if (room_count <= 0) {
    m_maxRoomPage = 0;
    return;
  }
  m_maxRoomPage =
      static_cast<int>(std::trunc((room_count - 1) / max_rooms_per_page));
}

StatusFormBase *MainWindow::getStatus(
    const StatusListHelper::Status &currentStatus) {
  statusForm::constructorItems items{*m_services,        *m_dataExchange,
                                     m_powerHandler,     *m_informationReader,
                                     currentStatus.room, m_roomRotationHandler};
  switch (currentStatus.type) {
    case StatusListHelper::StatusType::configuration:
      return new MoldForm(items, this);
    case StatusListHelper::StatusType::co2:
      return new Co2Form(items, currentStatus.sensors.front(),
                         m_co2_state_handler, this);
    case StatusListHelper::StatusType::generic:
      return new GenericValueForm(items, currentStatus.sensors.front(), this);
    case StatusListHelper::StatusType::particles:
      return new ParticlesForm(items, currentStatus.sensors, this);
    case StatusListHelper::StatusType::current_clamp:
      return new CurrentClampForm(items, currentStatus.sensors, this);
  }
  return nullptr;
}

void MainWindow::removeValvesFromSensors(MainWindow::sensor_ids &sensors) {
  const auto enocean_ids_to_remove =
      heating_valve_helper::get_all_heating_valve_enocean_ids(sensors);
  const auto new_end = std::remove_if(
      sensors.begin(), sensors.end(),
      [&enocean_ids_to_remove](const wolf::sensor_id &id) {
        const auto enocean_id = wolf::sensor_id_enocean::convert_to_esp3_id(id);
        return heating_valve_helper::is_enocean_id_in_list(
            enocean_id, enocean_ids_to_remove);
      });
  sensors.erase(new_end, sensors.end());
}

std::pair<wolf::room, configuration> MainWindow::updateRoom(
    const wolf::room &room, const configuration &configuration_,
    const std::vector<wolf::sensor_profile> &profiles,
    const std::vector<wolf::sensor_value> &values,
    const MainWindow::sensor_ids &sensors) const {
  // TODO refactor into room_updater
  sensor_to_room_and_configuration_helper helper;
  auto room_copy(room);
  auto configuration_copy(configuration_);
  auto outdoor = m_dataExchange->get_outdoor_sensor_id();
  helper.handle(sensors, room_copy, configuration_copy,
                outdoor.temperature.is_set());
  sensor_setter_helper setter_helper{*m_services->room_and_configuration_saver,
                                     *m_services->learn_handler,
                                     *m_services->value_handler};
  return setter_helper.set_to_room_and_configuration(
      room_copy, configuration_copy, profiles, values);
}

void MainWindow::handleEnoceanProfilesRoom(
    const wolf::types::id_esp3 &enocean_sensor_id,
    const std::array<uint8_t, 3> &eep,
    const MainWindow::sensor_ids &learned_ids,
    std::vector<wolf::sensor_profile> &profiles,
    MainWindow::sensor_ids &new_ids) const {
  const auto registered_profiles =
      m_services->enocean_handler->register_(enocean_sensor_id, eep, false);
  // enocean gets the profiles for all sensor types from the eep, but we
  // do not use all, so only the ones that are used should be learned
  std::copy_if(registered_profiles.cbegin(), registered_profiles.cend(),
               std::back_inserter(profiles),
               [&learned_ids](const wolf::sensor_profile &profile) {
                 for (const auto &check_id : learned_ids)
                   if (check_id == profile.id) return true;
                 return false;
               });
  assert(learned_ids.size() == profiles.size());
  new_ids = learned_ids;
  if (m_services->config_wolf->general_.send_sensor_rssi) {
    auto rssi_id = wolf::sensor_id_rssi::create_from_esp3_id(enocean_sensor_id);
    new_ids.push_back(rssi_id);
  }
}

void MainWindow::openRoom(const wolf::room &room,
                          const configuration &configuration_,
                          const bool is_add, const sensor_values &values) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "MainWindow::openRoom(),"
         " room:"
      << room << " configuration:" << configuration_;
  wolf::thread_checker::throw_if_not_ui("MainWindow::openRoom");
  RoomForm *room_form = new RoomForm(*m_services, *m_dataExchange, room,
                                     configuration_, m_roomNameHandler, this);
  connect(room_form, &RoomForm::back, this,
          [this, room_form, values, is_add]() {
            asyncDisableAndSaveRoomFormAndValues(
                room_form,
                [this, is_add](const wolf::room &, const configuration &) {
                  if (is_add) changeToLastPage();
                  openRooms();
                },
                values);
          });
  connect(room_form, &RoomForm::removed, this, [this]() {
    if (m_roomPage == m_maxRoomPage) changeToLastPage();
    openRooms();
  });
  connect(room_form, &RoomForm::changeSensor, this, [this, room_form]() {
    asyncDisableAndSaveRoomForm(
        room_form, [this](const wolf::room &room, const configuration &config) {
          openChangeSensor(room, config);
        });
  });
  connect(room_form, &RoomForm::changeControl, this, [this, room_form]() {
    asyncDisableAndSaveRoomForm(
        room_form, [this](const wolf::room &room, const configuration &) {
          openChangeControl(room);
        });
  });
  connect(room_form, &RoomForm::changeSensorOffset, this, [this, room_form]() {
    asyncDisableAndSaveRoomForm(
        room_form, [this](const wolf::room &room, const configuration &config) {
          openChangeSensorOffset(room, config);
        });
  });
  setMainForm(room_form);
}

void MainWindow::openNewRoom() {
  openChangeSensor(wolf::room(), configuration());
}

void MainWindow::openOptions() {
  OptionsForm *options = new OptionsForm(this);
  connect(options, &OptionsForm::back, this, &MainWindow::openSettings);
  connect(options, &OptionsForm::openDisplayOptions, this,
          &MainWindow::openDisplayOptions);
  connect(options, &OptionsForm::openFrsiOptions, this,
          &MainWindow::openFrsiOptions);
  connect(options, &OptionsForm::openResetOptions, this,
          &MainWindow::openResetOptions);
  setMainForm(options);
}

void MainWindow::openDisplayOptions() {
  DisplayOptions *options =
      new DisplayOptions(*m_services, *m_dataExchange, this);
  connect(options, &DisplayOptions::back, this, &MainWindow::openOptions);
  connect(options, &DisplayOptions::openQuestionnaireOptions, this,
          &MainWindow::openComfortQuestionnaireOptions);
  setMainForm(options);
}

void MainWindow::openFrsiOptions() {
  FrsiOptions *options = new FrsiOptions(*m_services, this);
  connect(options, &FrsiOptions::back, this, &MainWindow::openOptions);
  connect(options, &FrsiOptions::setPerRoom, this,
          [this] { openOptionsPerRoomList(); });
  setMainForm(options);
}

void MainWindow::openResetOptions() {
  ResetOptions *options = new ResetOptions(*m_services, this);
  connect(options, &ResetOptions::back, this, &MainWindow::openOptions);
  setMainForm(options);
}

void MainWindow::openOptionsPerRoomList() {
  calculateMaxPage();
  RoomsForm *rooms =
      new RoomsForm(*m_dataExchange, false, 0, m_maxRoomPage, this);
  connect(rooms, &RoomsForm::back, this, &MainWindow::openOptions);
  connect(rooms, &RoomsForm::openRoom,
          [this](const wolf::types::uuid_array &room_id) {
            openOptionsPerRoom(room_id);
          });
  setMainForm(rooms);
}

void MainWindow::openOptionsPerRoom(const wolf::types::uuid_array &room_id) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "MainWindow::openOptionsPerRoom(), room_id:" << room_id;
  const auto roomAndConfig = getRoomAndConfiguration(room_id);
  BOOST_ASSERT_MSG(roomAndConfig, "can't openOptionsPerRoom");
  if (!roomAndConfig) {
    LOGGING_LOG(m_logger, severity::error)
        << "failed to open options for a room";
    return;
  }
  OptionsPerRoomForm *options = new OptionsPerRoomForm(
      *m_services, roomAndConfig.get().second.mold_settings_,
      roomAndConfig.get().first.name, this);
  const auto handleBack = [this, options, config = roomAndConfig.get().second] {
    options->setDisabled(true);
    auto config_copy = config;
    config_copy.mold_settings_ = options->get();
    m_services->service->post(
        [handler = m_services->configurations, config_copy, this] {
          try {
            handler->update(config_copy);
          } catch (const std::exception &error) {
            LOGGING_LOG(m_logger, severity::error)
                << "OptionsPerRoomForm, configuration_handler->update "
                   "failed with a fatal error. "
                   "what():"
                << error.what();
          }
          postToGUIThread([this] { openOptionsPerRoomList(); });
        });
  };
  connect(options, &OptionsPerRoomForm::back, this, handleBack);
  setMainForm(options);
}

void MainWindow::openOutdoor() {
  OutDoorForm *outdoor = new OutDoorForm(*m_services, *m_dataExchange, this);
  connect(outdoor, &OutDoorForm::back, this, &MainWindow::openSettings);
  connect(outdoor, &OutDoorForm::chooseForeignSensor, this,
          [this]() { openForeignSensorOutdoor(); });
  connect(outdoor, &OutDoorForm::sensorOffset, this,
          [this]() { openChangeSensorOffsetOutdoor(); });
  setMainForm(outdoor);
}

void MainWindow::openChangeSensor(const wolf::room &room,
                                  const configuration &configuration_) {
  LearnSensorForm *learn =
      new LearnSensorForm(*m_services, *m_dataExchange, this);
  connect(learn, &LearnSensorForm::back, [this, learn, room, configuration_]() {
    const bool is_add = room.id == wolf::types::uuid_array{};
    const auto enocean_sensor_id = learn->getLastEnoceanId();
    if (enocean_sensor_id == 0) {
      if (is_add) {
        // new room got canceled
        openRooms();
        return;
      }
      openRoom(room, configuration_, false);
      return;
    }
    const auto eep = learn->getEep();
    const auto values = learn->getValues();
    const auto ids = learn->getLearnedIds();
    m_current->setEnabled(false);

    m_services->service->post([room, configuration_, eep, enocean_sensor_id,
                               ids, values, is_add, this] {
      // TODO refactor the whole callback to its own class
      wolf::room room_result;
      configuration configuration_result;
      try {
        std::vector<wolf::sensor_profile> profiles;
        sensor_ids new_sensors;
        handleEnoceanProfilesRoom(enocean_sensor_id, eep, ids, profiles,
                                  new_sensors);
        for (const auto &profile : profiles)
          m_services->learn_handler->learn(profile);
        if (is_add) {
          room_creator helper{*m_services->rooms, *m_services->configurations};
          wolf::uuid_helper uuid_helper;
          const auto initial_id =
              uuid_helper.string_to_array(m_services->config->initial_.room_id);
          const auto result = helper.create(
              wolf::types::uuid_array{}, m_roomNameHandler.getDefaultRoomName(),
              new_sensors, initial_id, false);
          room_result = result.room;
          configuration_result = result.configuration_;
        } else {
          const auto room_config =
              updateRoom(room, configuration_, profiles, values, new_sensors);
          room_result = room_config.first;
          configuration_result = room_config.second;
        }
        m_services->value_handler->handle_list(values);
      } catch (const std::runtime_error &error) {
        LOGGING_LOG(m_logger, logging::severity::error)
            << "failed to save new sensor, what():" << error.what();
      }
      postToGUIThread(
          [room_result, configuration_result, is_add, values, this] {
            openRoom(room_result, configuration_result, is_add, values);
          });
    });
  });
  setMainForm(learn);
}

void MainWindow::openChangeControl(const wolf::room &room) {
  Controller *control = new Controller(room, this);
  connect(control, &Controller::back, this, [this, control]() {
    const auto &room = control->getRoom();
    openRoom(room.id);
  });
  connect(control, &Controller::changeSVAS1, this, [this, control]() {
    const auto &room = control->getRoom();
    openChangeSVAS(room, ControllerTypes::sVAS1);
  });
  connect(control, &Controller::changeSVAS2, this, [this, control]() {
    const auto &room = control->getRoom();
    openChangeSVAS(room, ControllerTypes::sVAS2);
  });
  connect(control, &Controller::changeSVAS3, this, [this, control]() {
    const auto &room = control->getRoom();
    openChangeSVAS(room, ControllerTypes::sVAS3);
  });
  connect(control, &Controller::changeSVAS4, this, [this, control]() {
    const auto &room = control->getRoom();
    openChangeSVAS(room, ControllerTypes::sVAS4);
  });
  setMainForm(control);
}

void MainWindow::openChangeSensorOffset(const wolf::room &room,
                                        const configuration &configuration_) {
  const auto value_offsets = m_services->sensor_value_offset_handler;
  // TODO this getter must not be called from ui code.
  const float offset_temperature =
      value_offsets->get(configuration_.temperature).value_or(0.f);
  const float offset_humidity =
      value_offsets->get(configuration_.humidity).value_or(0.f);
  SensorOffsetForm *sensor_offset =
      new SensorOffsetForm({offset_temperature, offset_humidity}, true, this);
  connect(
      sensor_offset, &SensorOffsetForm::back, this,
      [this, room, configuration_, sensor_offset]() {
        m_services->service->post([room, configuration_,
                                   offset = sensor_offset->getOffset(), this] {
          const auto value_offsets = m_services->sensor_value_offset_handler;
          value_offsets->set({configuration_.temperature, offset.temperature});
          value_offsets->set({configuration_.humidity, offset.humidity});
        });
        openRoom(room, configuration_, false);
      });
  setMainForm(sensor_offset);
}

void MainWindow::openChangeSensorOffsetOutdoor() {
  const auto outdoor_sensor = m_dataExchange->get_outdoor_sensor_id();
  const auto value_offsets = m_services->sensor_value_offset_handler;
  // TODO this getter must not be called from ui code.
  const float offset_temperature =
      value_offsets->get(outdoor_sensor.temperature).value_or(0.f);
  const float offset_humidity =
      value_offsets->get(outdoor_sensor.humidity).value_or(0.f);
  const SensorOffsetForm::offsets offsets{offset_temperature, offset_humidity};
  SensorOffsetForm *sensor_offset = new SensorOffsetForm(offsets, true, this);
  connect(
      sensor_offset, &SensorOffsetForm::back, this,
      [this, sensor_offset, outdoor_sensor]() {
        m_services->service->post(
            [handler = m_services->sensor_value_offset_handler,
             offset = sensor_offset->getOffset(), outdoor_sensor] {
              handler->set({outdoor_sensor.temperature, offset.temperature});
              handler->set({outdoor_sensor.humidity, offset.humidity});
            });
        openSettings();
      });
  setMainForm(sensor_offset);
}

void MainWindow::openForeignSensorOutdoor() {
  ForeignSensorForm *form =
      new ForeignSensorForm(*m_services, *m_dataExchange, this);
  connect(form, &ForeignSensorForm::back, [this, form]() {
    const auto profiles = form->get_choosen_profiles();
    openSettings();
    m_services->service->post([this, profiles] {
      try {
        wolf::outdoor_sensor outdoor;
        for (const auto &profile : profiles) {
          m_services->learn_handler->learn(profile);
          if (profile.id.get_type() == wolf::sensor_value_type::temperature)
            outdoor.temperature = profile.id;
          if (profile.id.get_type() == wolf::sensor_value_type::humidity)
            outdoor.humidity = profile.id;
        }
        if (!outdoor.is_set()) return;
        m_services->outdoor_change_helper->set(outdoor);
      } catch (const std::runtime_error &error) {
        LOGGING_LOG(m_logger, logging::severity::error)
            << "could not set foreignSensorOutdoor, error:" << error.what();
      }
    });
    openSettings();
  });
  setMainForm(form);
}

void MainWindow::openChangeSVAS(const wolf::room &room,
                                const ControllerTypes &controller_) {
  ControllerSettings *settings = new ControllerSettings(
      *m_services, *m_dataExchange, controller_, room.id, this);
  connect(settings, &ControllerSettings::back, this,
          [this, room]() { openChangeControl(room); });
  connect(settings, &ControllerSettings::changeActors, this,
          [this, room, controller_]() { openChangeActors(room, controller_); });
  connect(settings, &ControllerSettings::setLimits, this,
          [this, room, controller_]() {
            if (controller_ == ControllerTypes::sVAS4)
              openChangeLimits(room, controller_, LimitTypes::CO2);
            else
              openChangeLimits(room, controller_, LimitTypes::humidity);
          });
  setMainForm(settings);
}

void MainWindow::openChangeActors(const wolf::room &room,
                                  const ControllerTypes &controller_) {
  Actors *actors =
      new Actors(*m_services, *m_dataExchange, controller_, room.id, this);
  if (controller_ == ControllerTypes::sVAS2)
    actors->changeActorDescription(tr("Dehumidifier:"));
  else if (controller_ == ControllerTypes::sVAS3)
    actors->changeActorDescription(tr("Heating:"));
  connect(actors, &Actors::back, this,
          [this, room, controller_]() { openChangeSVAS(room, controller_); });
  connect(actors, &Actors::openActorList1, this, [this, room, controller_]() {
    openActorList(room, controller_, 0, 1);
  });
  connect(actors, &Actors::openActorList2, this, [this, room, controller_]() {
    openActorList(room, controller_, 1, 1);
  });
  connect(actors, &Actors::openActorList3, this, [this, room, controller_]() {
    openActorList(room, controller_, 2, 1);
  });
  connect(actors, &Actors::openActorList4, this, [this, room, controller_]() {
    openActorList(room, controller_, 3, 1);
  });
  setMainForm(actors);
}

void MainWindow::openActorList(const wolf::room &room,
                               const ControllerTypes &controller_,
                               const int button, const int page) {
  ActorList *actor_list = new ActorList(controller_, button, page, this);
  auto actor_ids = actor_list->getNewActorIds();
  connect(actor_list, &ActorList::back, this,
          [this, room, controller_]() { openChangeActors(room, controller_); });
  connect(actor_list, &ActorList::chooseActor1, this,
          [this, room, controller_, button, actor_ids]() {
            openLearnActor(room, controller_, button, actor_ids[0]);
          });
  connect(actor_list, &ActorList::chooseActor2, this,
          [this, room, controller_, button, actor_ids]() {
            openLearnActor(room, controller_, button, actor_ids[1]);
          });
  connect(actor_list, &ActorList::chooseActor3, this,
          [this, room, controller_, button, actor_ids]() {
            openLearnActor(room, controller_, button, actor_ids[2]);
          });
  connect(actor_list, &ActorList::chooseActor4, this,
          [this, room, controller_, button, actor_ids]() {
            openLearnActor(room, controller_, button, actor_ids[3]);
          });
  connect(actor_list, &ActorList::previous, this,
          [this, actor_list, room, controller_, button]() {
            auto page = actor_list->getPage();
            openActorList(room, controller_, button, page);
          });
  connect(actor_list, &ActorList::next, this,
          [this, actor_list, room, controller_, button]() {
            auto page = actor_list->getPage();
            openActorList(room, controller_, button, page);
          });
  setMainForm(actor_list);
}

void MainWindow::openLearnActor(const wolf::room &room,
                                const ControllerTypes &controller_,
                                const int button, const int actor_id) {
  LearnActorForm *learn_actor = new LearnActorForm(
      *m_services, controller_, room.id, button, actor_id, this);
  connect(learn_actor, &LearnActorForm::back, this,
          [this, learn_actor, room, controller_, button]() {
            if (learn_actor->isLearned())
              openChangeActors(room, controller_);
            else
              openActorList(room, controller_, button, 1);
          });
  setMainForm(learn_actor);
}

void MainWindow::openChangeLimits(const wolf::room &room,
                                  const ControllerTypes &controller_,
                                  const LimitTypes &type) {
  Limits *limits = new Limits(*m_services, *m_dataExchange, type, controller_,
                              room.id, this);
  connect(limits, &Limits::back, this,
          [this, room, controller_]() { openChangeSVAS(room, controller_); });
  connect(limits, &Limits::next, this, [this, room, controller_, type]() {
    handleNextLimits(room, controller_, type);
  });
  connect(limits, &Limits::previous, this, [this, room, controller_, type]() {
    handlePreviousLimits(room, controller_, type);
  });
  setMainForm(limits);
}

void MainWindow::openDebugStatusScreen() {
  DebugStatusScreen *debug = new DebugStatusScreen(*m_services, this);
  connect(debug, &DebugStatusScreen::back, this, &MainWindow::openLanguage);
  setMainForm(debug);
}

void MainWindow::openWifiScreen(const access_point &to_connect,
                                const std::string password) {
  WifiForm *wifi =
      new WifiForm(*m_services, m_dataExchange, to_connect, password, this);
  connect(wifi, &WifiForm::back, this, &MainWindow::openSettings);
  connect(wifi, &WifiForm::accessPointChosen, this,
          &MainWindow::openWifiConnect);
  setMainForm(wifi);
}

void MainWindow::openWifiConnect(const MainWindow::access_point &to_connect) {
  WifiConnectForm *wifi_connect = new WifiConnectForm(to_connect, this);
  connect(wifi_connect, &WifiConnectForm::back, this,
          [this]() { openWifiScreen(make_invalid_access_point(), ""); });
  connect(wifi_connect, &WifiConnectForm::connecting, this,
          [this, to_connect, wifi_connect]() {
            openWifiScreen(to_connect, wifi_connect->getPassword());
          });
  setMainForm(wifi_connect);
}

void MainWindow::openSensorActivityLog() {
  SensorActivityLog *activity_log =
      new SensorActivityLog(m_sensor_activity_handler, this);
  connect(activity_log, &SensorActivityLog::back, this,
          &MainWindow::openSettings);
  setMainForm(activity_log);
}

void MainWindow::openPinProtection(const bool change_pin) {
  PinProtectionForm *pin_protection =
      new PinProtectionForm(*m_services, change_pin, this);
  if (!change_pin && pin_protection->checkIfEmpty())
    openSettings();
  else {
    connect(pin_protection, &PinProtectionForm::back, this,
            [change_pin, this]() {
              if (change_pin)
                openSettings();
              else
                openStatus();
            });
    connect(pin_protection, &PinProtectionForm::correctPin, this,
            [this]() { openSettings(); });
    setMainForm(pin_protection);
  }
}

void MainWindow::jumpToWarningScreen(const wolf::types::uuid_array &id,
                                     const bool is_co2) {
  const StatusListHelper helper(*m_dataExchange);
  const auto statusList = helper.getStatusList();
  for (unsigned int index = 0; index < statusList.size(); ++index) {
    const auto &status = statusList[index];
    if (status.room == id) {
      if ((status.type == StatusListHelper::StatusType::co2 && is_co2) ||
          (status.type != StatusListHelper::StatusType::co2 && !is_co2))
        m_statusIndex = static_cast<int>(index);
    }
  }
}

void MainWindow::openComfortQuestionnaireOptions() {
  ComfortQuestionnaireOptions *options =
      new ComfortQuestionnaireOptions(*m_services, this);
  connect(options, &ComfortQuestionnaireOptions::back, this,
          &MainWindow::openDisplayOptions);
  connect(options, &ComfortQuestionnaireOptions::startQuestionnaire, this,
          &MainWindow::openComfortQuestionnaire);
  setMainForm(options);
}

void MainWindow::openComfortQuestionnaire() {
  ComfortQuestionnaire *questionnaire =
      new ComfortQuestionnaire(*m_services, this);
  connect(questionnaire, &ComfortQuestionnaire::back, this,
          &MainWindow::openStatus);
  setMainForm(questionnaire);
}

void MainWindow::openHeatingValveOptions(const wolf::room &room,
                                         const configuration &configuration_) {
  HeatingValveSettings *valve =
      new HeatingValveSettings(*m_services, room, this);
  connect(valve, &HeatingValveSettings::back, this, &MainWindow::openStatus);
  connect(valve, &HeatingValveSettings::openLearnPage, this,
          [this, valve, room, configuration_]() {
            const auto settings = valve->get_current_settings();
            if (!settings.has_value())
              LOGGING_LOG(m_logger, logging::severity::error)
                  << "heating_valve_settings should have a value, room: "
                  << room;
            const heating_valve_settings default_settings = {
                {}, false, false, 0.f, 0};
            openLearnHeatingValve(room, configuration_,
                                  settings.value_or(default_settings));
          });
  setMainForm(valve);
}

void MainWindow::openLearnHeatingValve(const wolf::room &room,
                                       const configuration &configuration_,
                                       const heating_valve_settings &settings) {
  LearnHeatingValve *learn_valve =
      new LearnHeatingValve(*m_services, settings, this);
  connect(learn_valve, &LearnHeatingValve::back,
          [this, learn_valve, room, configuration_]() {
            const bool is_add = room.id == wolf::types::uuid_array{};
            if (is_add)
              throw std::runtime_error(
                  "heating valve can only be added to existing room");
            const auto enocean_sensor_id = learn_valve->getLastEnoceanId();
            if (enocean_sensor_id == 0) {
              openHeatingValveOptions(room, configuration_);
              return;
            }
            const auto eep = learn_valve->getEep();
            const auto values = learn_valve->getValues();
            const auto ids = learn_valve->getLearnedIds();
            m_current->setEnabled(false);

            m_services->service->post([room, configuration_, eep,
                                       enocean_sensor_id, ids, values, this] {
              // TODO refactor the whole callback to its own class
              wolf::room room_result;
              configuration configuration_result;
              try {
                std::vector<wolf::sensor_profile> profiles;
                sensor_ids new_sensors;
                handleEnoceanProfilesRoom(enocean_sensor_id, eep, ids, profiles,
                                          new_sensors);
                auto sensors = room.sensors;
                for (const auto &new_sensor : new_sensors) {
                  if (std::find(sensors.cbegin(), sensors.cend(), new_sensor) ==
                      sensors.cend())
                    sensors.push_back(new_sensor);
                }
                const auto room_config =
                    updateRoom(room, configuration_, profiles, values, sensors);
                room_result = room_config.first;
                configuration_result = room_config.second;
                m_services->value_handler->handle_list(values);
              } catch (const std::runtime_error &error) {
                LOGGING_LOG(m_logger, logging::severity::error)
                    << "failed to save new heating valve, what():"
                    << error.what();
              }
              postToGUIThread([room_result, configuration_result, this] {
                openHeatingValveOptions(room_result, configuration_result);
              });
            });
          });
  connect(learn_valve, &LearnHeatingValve::removeValves,
          [this, room, configuration_]() {
            const bool is_add = room.id == wolf::types::uuid_array{};
            if (is_add)
              throw std::runtime_error(
                  "heating valve can only be removed from existing room");
            m_current->setEnabled(false);

            m_services->service->post([room, configuration_, this] {
              // TODO refactor the whole callback to its own class
              wolf::room room_result;
              configuration configuration_result;
              try {
                auto sensor_ids = room.sensors;
                removeValvesFromSensors(sensor_ids);
                const auto room_config =
                    updateRoom(room, configuration_, {}, {}, sensor_ids);
                room_result = room_config.first;
                configuration_result = room_config.second;
              } catch (const std::runtime_error &error) {
                LOGGING_LOG(m_logger, logging::severity::error)
                    << "failed to save new heating valve, what():"
                    << error.what();
              }
              postToGUIThread([room_result, configuration_result, this] {
                openHeatingValveOptions(room_result, configuration_result);
              });
            });
          });
  setMainForm(learn_valve);
}

bool MainWindow::replaceFirstSensor(std::vector<wolf::sensor_id> &container,
                                    const wolf::sensor_id &search,
                                    const wolf::sensor_id &replacement) {
  const auto found = std::find(container.begin(), container.end(), search);
  if (found == container.end()) {
    return false;
  }
  *found = replacement;
  return true;
}

void MainWindow::setMainForm(QWidget *form) {
  if (m_current) {
    m_current->deleteLater();
  }
  m_current = form;
  setCentralWidget(form);
}

boost::optional<std::pair<wolf::room, configuration>>
MainWindow::getRoomAndConfiguration(const wolf::types::uuid_array &room_id) {
  const boost::optional<wolf::room> room = m_dataExchange->get_room(room_id);
  if (!room) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not find room! room_id:" << room_id;
    return {};
  }
  const auto config = m_dataExchange->get_configuration_by_room(room_id);
  if (!config) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not find any configuration for room:" << room_id;
    return {};
  }
  return {{room.get(), config.get()}};
}

void MainWindow::handleLimitsSVAS1(const wolf::room &room,
                                   const ControllerTypes &controller_,
                                   const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::humidityBorder:
    case LimitTypes::temperature:
    case LimitTypes::CO2:
    case LimitTypes::sproutValue:
    case LimitTypes::VOC:
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::humidity);
      break;
  }
}

void MainWindow::handleNextLimitsSVAS2(const wolf::room &room,
                                       const ControllerTypes &controller_,
                                       const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
      openChangeLimits(room, controller_, LimitTypes::humidityBorder);
      break;
    case LimitTypes::humidityBorder:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::temperature:
    case LimitTypes::CO2:
    case LimitTypes::sproutValue:
    case LimitTypes::VOC:
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::humidity);
      break;
  }
}

void MainWindow::handlePreviousLimitsSVAS2(const wolf::room &room,
                                           const ControllerTypes &controller_,
                                           const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::humidityBorder:
      openChangeLimits(room, controller_, LimitTypes::humidity);
      break;
    case LimitTypes::temperature:
    case LimitTypes::CO2:
    case LimitTypes::sproutValue:
    case LimitTypes::VOC:
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::humidityBorder);
      break;
  }
}

void MainWindow::handleNextLimitsSVAS3(const wolf::room &room,
                                       const ControllerTypes &controller_,
                                       const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
      openChangeLimits(room, controller_, LimitTypes::temperature);
      break;
    case LimitTypes::temperature:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::humidityBorder:
    case LimitTypes::CO2:
    case LimitTypes::sproutValue:
    case LimitTypes::VOC:
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::humidity);
      break;
  }
}

void MainWindow::handlePreviousLimitsSVAS3(const wolf::room &room,
                                           const ControllerTypes &controller_,
                                           const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::temperature:
      openChangeLimits(room, controller_, LimitTypes::humidity);
      break;
    case LimitTypes::humidityBorder:
    case LimitTypes::CO2:
    case LimitTypes::sproutValue:
    case LimitTypes::VOC:
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::temperature);
      break;
  }
}

void MainWindow::handlePreviousLimitsSVAS4(const wolf::room &room,
                                           const ControllerTypes &controller_,
                                           const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
    case LimitTypes::humidityBorder:
    case LimitTypes::temperature:
      break;
    case LimitTypes::CO2:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::sproutValue:
      openChangeLimits(room, controller_, LimitTypes::CO2);
      break;
    case LimitTypes::VOC:
      openChangeLimits(room, controller_, LimitTypes::sproutValue);
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::VOC);
      break;
  }
}

void MainWindow::handleNextLimitsSVAS4(const wolf::room &room,
                                       const ControllerTypes &controller_,
                                       const LimitTypes &type) {
  switch (type) {
    case LimitTypes::humidity:
    case LimitTypes::humidityBorder:
    case LimitTypes::temperature:
      break;
    case LimitTypes::CO2:
      openChangeLimits(room, controller_, LimitTypes::sproutValue);
      break;
    case LimitTypes::sproutValue:
      openChangeLimits(room, controller_, LimitTypes::VOC);
      break;
    case LimitTypes::VOC:
      openChangeLimits(room, controller_, LimitTypes::outdoorTemperature);
      break;
    case LimitTypes::outdoorTemperature:
      openChangeLimits(room, controller_, LimitTypes::CO2);
      break;
  }
}

void MainWindow::handleNextLimits(const wolf::room &room,
                                  const ControllerTypes &controller_,
                                  const LimitTypes &type) {
  switch (controller_) {
    case ControllerTypes::sVAS1:
      handleLimitsSVAS1(room, controller_, type);
      break;
    case ControllerTypes::sVAS2:
      handleNextLimitsSVAS2(room, controller_, type);
      break;
    case ControllerTypes::sVAS3:
      handleNextLimitsSVAS3(room, controller_, type);
      break;
    case ControllerTypes::sVAS4:
      handleNextLimitsSVAS4(room, controller_, type);
      break;
  }
}

void MainWindow::handlePreviousLimits(const wolf::room &room,
                                      const ControllerTypes &controller_,
                                      const LimitTypes &type) {
  switch (controller_) {
    case ControllerTypes::sVAS1:
      handleLimitsSVAS1(room, controller_, type);
      break;
    case ControllerTypes::sVAS2:
      handlePreviousLimitsSVAS2(room, controller_, type);
      break;
    case ControllerTypes::sVAS3:
      handlePreviousLimitsSVAS3(room, controller_, type);
      break;
    case ControllerTypes::sVAS4:
      handlePreviousLimitsSVAS4(room, controller_, type);
      break;
  }
}

MainWindow::access_point MainWindow::make_invalid_access_point() {
  return {"", "", 0, false};
}
