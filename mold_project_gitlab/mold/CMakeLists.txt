
set (UI_FILES
    gui/ActorList.ui
    gui/Actors.ui
    gui/Co2Form.ui
    gui/Co2ProgressWidget.ui
    gui/ComfortQuestionnaire.ui
    gui/ComfortQuestionnaireOptions.ui
    gui/Confirmation.ui
    gui/Controller.ui
    gui/ControllerSettings.ui
    gui/ChangeLanguageForm.ui
    gui/CurrentClampForm.ui
    gui/DebugStatusScreen.ui
    gui/DisplayOptions.ui
    gui/ForeignSensorForm.ui
    gui/FrsiOptions.ui
    gui/GenericValueForm.ui
    gui/HeatingValveSettings.ui
    gui/LearnActorForm.ui
    gui/LearnHeatingValve.ui
    gui/LearnSensorForm.ui
    gui/Limits.ui
    gui/MainWindow.ui
    gui/MoldForm.ui
    gui/MoldValueProgressWidget.ui
    gui/MoldValuesForm.ui
    gui/NumberGeneric.ui
    gui/NumberHumidity.ui
    gui/NumberTemperature.ui
    gui/OptionsForm.ui
    gui/OptionsPerRoomForm.ui
    gui/OutDoorForm.ui
    gui/ParticlesForm.ui
    gui/PinProtectionForm.ui
    gui/ResetOptions.ui
    gui/RoomForm.ui
    gui/RoomHeaderForm.ui
    gui/RoomsForm.ui
    gui/SensorOffsetForm.ui
    gui/SensorActivityLog.ui
    gui/SettingsForm.ui
    gui/StatusBarForm.ui
    gui/WifiConnectForm.ui
    gui/WifiForm.ui
)

# qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(UIS_HDRS
    ${UI_FILES}
)

set (SOURCE_FILES_MOLD_GUI
    gui/ActorList.cpp gui/ActorList.hpp
    gui/Actors.cpp gui/Actors.hpp
    gui/ButtonDisableHandler.cpp gui/ButtonDisableHandler.hpp
    gui/ChangeLanguageForm.cpp gui/ChangeLanguageForm.hpp
    gui/Co2Form.cpp gui/Co2Form.hpp
    gui/Co2ProgressWidget.cpp gui/Co2ProgressWidget.hpp
    gui/Co2StateHandler.cpp gui/Co2StateHandler.hpp
    gui/ComfortQuestionnaire.cpp gui/ComfortQuestionnaire.hpp
    gui/ComfortQuestionnaireOptions.cpp gui/ComfortQuestionnaireOptions.hpp
    gui/ComfortQuestionnaireQuestionTypes.hpp
    gui/Confirmation.cpp gui/Confirmation.hpp
    gui/Controller.cpp gui/Controller.hpp
    gui/ControllerLabelHandler.cpp gui/ControllerLabelHandler.hpp
    gui/ControllerSettings.cpp gui/ControllerSettings.hpp
    gui/ControllerTypes.hpp
    gui/CurrentClampForm.cpp gui/CurrentClampForm.hpp
    gui/DataExchange.cpp gui/DataExchange.hpp
    gui/DebugStatusScreen.cpp gui/DebugStatusScreen.hpp
    gui/DisplayOptions.cpp gui/DisplayOptions.hpp
    gui/DisplayPowerHandler.cpp gui/DisplayPowerHandler.hpp
    gui/ForeignSensorForm.cpp gui/ForeignSensorForm.hpp
    gui/FrsiOptions.cpp gui/FrsiOptions.hpp
    gui/GenericValueForm.cpp gui/GenericValueForm.hpp
    gui/GsmInformationReader.cpp gui/GsmInformationReader.hpp
    gui/HeatingValveSettings.cpp gui/HeatingValveSettings.hpp
    gui/InvisibleClickableButton.cpp gui/InvisibleClickableButton.hpp
    gui/LanguageHandler.cpp gui/LanguageHandler.hpp
    gui/LearnActorForm.cpp gui/LearnActorForm.hpp
    gui/LearnHeatingValve.cpp gui/LearnHeatingValve.hpp
    gui/LearnSensorForm.cpp gui/LearnSensorForm.hpp
    gui/LimitTypes.hpp
    gui/Limits.cpp gui/Limits.hpp
    gui/MainWindow.cpp gui/MainWindow.hpp
    gui/MoldForm.cpp gui/MoldForm.hpp
    gui/MoldValuesForm.cpp gui/MoldValuesForm.hpp
    gui/MoldValueProgressWidget.cpp gui/MoldValueProgressWidget.hpp
    gui/NumberBase.cpp gui/NumberBase.hpp
    gui/NumberFactory.hpp gui/NumberFactory.cpp
    gui/NumberGeneric.cpp gui/NumberGeneric.hpp
    gui/NumberHumidity.cpp gui/NumberHumidity.hpp
    gui/NumberTemperature.cpp gui/NumberTemperature.hpp
    gui/OptionsForm.cpp gui/OptionsForm.hpp
    gui/OptionsPerRoomForm.cpp gui/OptionsPerRoomForm.hpp
    gui/OutDoorForm.cpp gui/OutDoorForm.hpp
    gui/ParticlesForm.cpp gui/ParticlesForm.hpp
    gui/PinProtectionForm.cpp gui/PinProtectionForm.hpp
    gui/PostToGUIThread.hpp
    gui/ResetOptions.cpp gui/ResetOptions.hpp
    gui/RoomForm.cpp gui/RoomForm.hpp
    gui/RoomHeaderForm.cpp gui/RoomHeaderForm.hpp
    gui/RoomNameHandler.cpp gui/RoomNameHandler.hpp
    gui/RoomRotationHandler.cpp gui/RoomRotationHandler.hpp
    gui/RoomsForm.cpp gui/RoomsForm.hpp
    gui/SensorActivity.cpp gui/SensorActivity.hpp
    gui/SensorActivityHandler.cpp gui/SensorActivityHandler.hpp
    gui/SensorActivityLog.cpp gui/SensorActivityLog.hpp
    gui/SensorOffsetForm.cpp gui/SensorOffsetForm.hpp
    gui/SensorValueTypeToText.cpp gui/SensorValueTypeToText.hpp
    gui/SettingsForm.cpp gui/SettingsForm.hpp
    gui/SoundOnPressHandler.cpp gui/SoundOnPressHandler.hpp
    gui/StatusBarForm.cpp gui/StatusBarForm.hpp
    gui/StatusFormBase.cpp gui/StatusFormBase.hpp
    gui/StatusListHelper.cpp gui/StatusListHelper.hpp
    gui/ValueButton.hpp
    gui/ValueCheckHelper.cpp gui/ValueCheckHelper.hpp
    gui/WifiConnectForm.cpp gui/WifiConnectForm.hpp
    gui/WifiErrorType.hpp
    gui/WifiForm.cpp gui/WifiForm.hpp
    gui/WifiHandler.cpp gui/WifiHandler.hpp
    gui/WolfServices.hpp
)

set (TRANSLATION_TS_FILES
    gui/translations/mold_de.ts
)

set (STYLESHEETS
    gui/stylesheets/stylesheet.qss
)

IF(NOT Qt5_LRELEASE_EXECUTABLE)
    MESSAGE(WARNING "could not find lrelease. Your build won't contain translations.")
ENDIF(NOT Qt5_LRELEASE_EXECUTABLE)

qt5_create_translation(mold_translations ${TRANSLATION_TS_FILES} ${SOURCE_FILES_MOLD_GUI} ${UI_FILES})

configure_file(${CMAKE_SOURCE_DIR}/cmake/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/cmake/mold_version.txt.in ${CMAKE_SOURCE_DIR}/../install/mold_version.txt @ONLY)

set (SOURCE_FILES_MOLD
    beep_when_not_confirmed_handler.cpp beep_when_not_confirmed_handler.hpp
    boost_serialize_configuration.hpp
    boost_serialize_configuration_state.hpp
    boost_serialize_mold_settings.hpp
    boost_serialize_sprout_value.hpp
    building_type.cpp building_type.hpp
    calculation_state.cpp calculation_state.hpp
    comfort_questionnaire.cpp comfort_questionnaire.hpp
    comfort_questionnaire_cache.cpp comfort_questionnaire_cache.hpp
    comfort_questionnaire_handler.cpp comfort_questionnaire_handler.hpp
    configuration.cpp configuration.hpp
    configuration_cache.cpp configuration_cache.hpp
    configuration_handler.cpp configuration_handler.hpp
    configuration_saver.cpp configuration_saver.hpp
    configuration_state.cpp configuration_state.hpp
    configuration_state_handler.cpp configuration_state_handler.hpp
    configuration_values.cpp configuration_values.hpp
    configuration_values_handler.cpp configuration_values_handler.hpp
    configuration_values_outdoor.cpp configuration_values_outdoor.hpp
    controller_handler.cpp controller_handler.hpp
    display_handler.cpp display_handler.hpp
    display_handler_base.hpp
    evaluation_humidity/handler.cpp evaluation_humidity/handler.hpp
    generate_sensor_id_from_config.cpp generate_sensor_id_from_config.hpp
    gsm_information_reader.cpp gsm_information_reader.hpp
    gui_logger.cpp gui_logger.hpp
    heating_valve_helper.cpp heating_valve_helper.hpp
    heating_valve_settings.cpp heating_valve_settings.hpp
    heating_valve_settings_cache.cpp heating_valve_settings_cache.hpp
    heating_valve_settings_handler.cpp heating_valve_settings_handler.hpp
    iccid_sender.cpp iccid_sender.hpp
    initial_room_creator.cpp initial_room_creator.hpp
    median_calculator.cpp median_calculator.hpp
    median_calculator_creator.cpp median_calculator_creator.hpp
    mold_handler.cpp mold_handler.hpp
    mold_settings.cpp mold_settings.hpp
    mold_value.cpp mold_value.hpp
    mold_value_handler.cpp mold_value_handler.hpp
    mqtt_cache.cpp mqtt_cache.hpp
    mqtt_handler.cpp mqtt_handler.hpp
    network/building_type_helper.cpp network/building_type_helper.hpp
    network/iccid_sender.cpp network/iccid_sender.hpp
    network/sprout_type_helper.cpp network/sprout_type_helper.hpp
    network/substrate_type_helper.cpp network/substrate_type_helper.hpp
    nightmode_activator.cpp nightmode_activator.hpp
    nightmode_handler.cpp nightmode_handler.hpp
    options_cache.cpp options_cache.hpp
    options_handler.cpp options_handler.hpp
    outdoor_sensor_change_helper.cpp outdoor_sensor_change_helper.hpp
    power_calculator.cpp power_calculator.hpp
    program_options_parser.cpp program_options_parser.hpp
    reconnect_handler.cpp reconnect_handler.hpp
    reset_handler.cpp reset_handler.hpp
    reset_mode.cpp reset_mode.hpp
    reset_reconnect_timer_on_signal.cpp reset_reconnect_timer_on_signal.hpp
    room_and_configuration_saver.cpp room_and_configuration_saver.hpp
    room_creator.cpp room_creator.hpp
    room_creator_from_profiles.cpp room_creator_from_profiles.hpp
    room_saver.cpp room_saver.hpp
    room_updater.cpp room_updater.hpp
    scd30_handler.cpp scd30_handler.hpp
    sensor_id_evaluation_humidity.cpp sensor_id_evaluation_humidity.hpp
    sensor_id_from_uuid_and_types.cpp sensor_id_from_uuid_and_types.hpp
    sensor_id_sprout.cpp sensor_id_sprout.hpp
    sensor_id_mqtt.cpp sensor_id_mqtt.hpp
    sensor_setter_helper.cpp sensor_setter_helper.hpp
    sensor_to_room_and_configuration_helper.cpp sensor_to_room_and_configuration_helper.hpp
    sht25_handler.cpp sht25_handler.hpp
    shutdown_handler.cpp shutdown_handler.hpp
    sprout/cache.cpp sprout/cache.hpp
    sprout/handler.cpp sprout/handler.hpp
    sprout/latest_value_helper.cpp sprout/latest_value_helper.hpp
    sprout_percentage_calculator.cpp sprout_percentage_calculator.hpp
    sprout_type.cpp sprout_type.hpp
    sprout_value.cpp sprout_value.hpp
    substrate_type.cpp substrate_type.hpp
    surface_temperature_calculator.cpp surface_temperature_calculator.hpp
    sync/box_settings/cache.cpp sync/box_settings/cache.hpp
    sync/box_settings/entity.cpp sync/box_settings/entity.hpp
    sync/box_settings/read/handler.cpp sync/box_settings/read/handler.hpp
    sync/box_settings/read/parser.cpp sync/box_settings/read/parser.hpp
    sync/box_settings/write/handler.cpp sync/box_settings/write/handler.hpp
    sync/box_settings/write/sender.cpp sync/box_settings/write/sender.hpp
    sync/login_handler.cpp sync/login_handler.hpp
    sync/mold_sprout/cache.cpp sync/mold_sprout/cache.hpp
    sync/mold_sprout/handler.cpp sync/mold_sprout/handler.hpp
    sync/mold_sprout/sender.cpp sync/mold_sprout/sender.hpp
    sync/resync_after_timeout.cpp sync/resync_after_timeout.hpp
    sync/resync_everything.cpp sync/resync_everything.hpp
    sync/room/cache.cpp sync/room/cache.hpp
    sync/room/deleted/handler.cpp sync/room/deleted/handler.hpp
    sync/room/deleted/sender.cpp sync/room/deleted/sender.hpp
    sync/room/settings/handler.cpp sync/room/settings/handler.hpp
    sync/room/settings/parser.cpp sync/room/settings/parser.hpp
    sync/room/settings/with_sensors/handler.cpp sync/room/settings/with_sensors/handler.hpp
    sync/room/settings/with_sensors/sender.cpp sync/room/settings/with_sensors/sender.hpp
    sync/sensor_id_frsi.cpp sync/sensor_id_frsi.hpp
    sync/sensor_id_mold_state.cpp sync/sensor_id_mold_state.hpp
    sync/sensor_id_substrate.cpp sync/sensor_id_substrate.hpp
    sync/sensors_values/read/handler.cpp sync/sensors_values/read/handler.hpp
    sync/sensors_values/read/parser.cpp sync/sensors_values/read/parser.hpp
    sync/sensors_values/write/cache.cpp sync/sensors_values/write/cache.hpp
    sync/sensors_values/write/frsi.cpp sync/sensors_values/write/frsi.hpp
    sync/sensors_values/write/handler.cpp sync/sensors_values/write/handler.hpp
    sync/sensors_values/write/mold_state.cpp sync/sensors_values/write/mold_state.hpp
    sync/sensors_values/write/sender.cpp sync/sensors_values/write/sender.hpp
    sync/sensors_values/write/substrate_type.cpp sync/sensors_values/write/substrate_type.hpp
    tone_enabled_handler.cpp tone_enabled_handler.hpp
    tone_handler.cpp tone_handler.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/version.cpp version.hpp

# qt
    ${mold_translations}
    ${SOURCE_FILES_MOLD_GUI}
    gui/icons.qrc
    ${STYLESHEETS}
    ${UI_FILES}
)

add_library(mold ${SOURCE_FILES_MOLD})
target_link_libraries(mold
    PUBLIC wolf
    PUBLIC wifi
    PUBLIC mosquittopp
    Qt5::Core Qt5::Gui Qt5::Widgets
)
target_include_directories(mold
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)
add_executable(mold-exec mold.cpp)
target_link_libraries(mold-exec
    PUBLIC mold
)
SET_TARGET_PROPERTIES(mold-exec PROPERTIES OUTPUT_NAME mold)
if (BUILD_SHARED_LIBS)
  install(TARGETS mold
      LIBRARY DESTINATION lib
      )
endif(BUILD_SHARED_LIBS)
install(TARGETS mold-exec
    RUNTIME DESTINATION bin
    )
install(FILES ${mold_translations}
    DESTINATION translations
    )
install(FILES ${mold_scripts}
    DESTINATION translations
    )
install(FILES ${STYLESHEETS} scripts/enviroment scripts/enviroment_dev
    DESTINATION .
    )
install(FILES scripts/qt.conf
    DESTINATION bin
    )
install(FILES scripts/exec-mold scripts/exec-gsm_mux scripts/reset_device scripts/reset_pin_code
    DESTINATION .
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )
install(FILES scripts/scripts/display_off scripts/scripts/display_on scripts/scripts/certificate_update
    DESTINATION scripts
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )
list(APPEND LOGGING_SCRIPTS 
    cleanup_persistent_dirs
    delete_oldest_files
    get_logs_for_upload
    get_priority.awk
    hourly_jobs
    log_environment
    logger
    save_crash_logs
    save_high_priority_logs
    to_system_log
    upload_log
    upload_logs
    )
foreach(script ${LOGGING_SCRIPTS})
    list(APPEND LOGGING_SCRIPT_PATHS "scripts/scripts/logging/${script}")
endforeach()

install(FILES ${LOGGING_SCRIPT_PATHS}
    DESTINATION scripts/logging
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )

