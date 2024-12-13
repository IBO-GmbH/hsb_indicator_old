set(SOURCE_FILES
    actor/actor.cpp actor/actor.hpp
    actor/actor_indexes.hpp
    actor/actor_setter.cpp actor/actor_setter.hpp
    std_terminate_handler.cpp std_terminate_handler.hpp
    actor/boost_serialize_actor.hpp
    actor/handler.cpp actor/handler.hpp
    actor/indexes.hpp
    actor/loop_usage_checker.cpp actor/loop_usage_checker.hpp
    actor/sensor_profile.cpp actor/sensor_profile.hpp
    base64.cpp base64.hpp
    boost_serialize_chrono_system_clock_timepoint.hpp
    boost_serialize_helper.hpp
    boost_serialize_sensor_id.hpp
    boost_serialize_sensor_metadata.hpp
    boost_serialize_sensor_profile.hpp
    command_handler.cpp command_handler.hpp
    controller/boost_serialize_chrono_system_clock_duration.hpp
    controller/boost_serialize_limits.hpp
    controller/boost_serialize_values_per_room.hpp
    controller/calculator.cpp controller/calculator.hpp
    controller/controller.cpp controller/controller.hpp
    controller/controller_cache.cpp controller/controller_cache.hpp
    controller/controller_handler.cpp controller/controller_handler.hpp
    controller/controller_per_room.hpp
    controller/controller_ventilation.cpp controller/controller_ventilation.hpp
    controller/limits.cpp controller/limits.hpp
    controller/on_off_control.cpp controller/on_off_control.hpp
    controller/on_off_state.cpp controller/on_off_state.hpp
    controller/sensor_reader.cpp controller/sensor_reader.hpp
    controller/sensor_value_getter.cpp controller/sensor_value_getter.hpp
    controller/svas1_handler.cpp controller/svas1_handler.hpp
    controller/svas1_ventilation.cpp controller/svas1_ventilation.hpp
    controller/svas2_dehumidifier.cpp controller/svas2_dehumidifier.hpp
    controller/svas2_handler.cpp controller/svas2_handler.hpp
    controller/svas3_handler.cpp controller/svas3_handler.hpp
    controller/svas3_heating.cpp controller/svas3_heating.hpp
    controller/svas4_air_quality.cpp controller/svas4_air_quality.hpp
    controller/svas4_handler.cpp controller/svas4_handler.hpp
    controller/threshold_setter.cpp controller/threshold_setter.hpp
    controller/thresholds.cpp controller/thresholds.hpp
    controller/type.cpp controller/type.hpp
    controller/values_per_room.cpp controller/values_per_room.hpp
    database.cpp database.hpp
    database_cache.cpp database_cache.hpp
    database_transaction_helper.cpp database_transaction_helper.hpp
    ee895_handler.cpp ee895_handler.hpp
    ee895_parser.cpp ee895_parser.hpp
    ee895_reader.cpp ee895_reader.hpp
    enocean_cache.cpp enocean_cache.hpp
    enocean_channel_helper.cpp enocean_channel_helper.hpp
    enocean_handler.cpp enocean_handler.hpp
    enocean_link.cpp enocean_link.hpp
    enocean_subchannel_type.cpp enocean_subchannel_type.hpp
    eoprofile_stm550.cpp eoprofile_stm550.hpp
    esp3_commands.cpp esp3_commands.hpp
    esp3_crc.cpp esp3_crc.hpp
    esp3_parser.cpp esp3_parser.hpp
    esp3_sender.cpp esp3_sender.hpp
    foreign_sensor.cpp foreign_sensor.hpp
    hardware_address.cpp hardware_address.hpp
    i2c/device.hpp
    i2c/open_close.cpp i2c/open_close.hpp
    i2c/read_write.cpp i2c/read_write.hpp
    key_value_cache.cpp key_value_cache.hpp
    languages.hpp
    login_handler.cpp login_handler.hpp
    low_pass_filter.cpp low_pass_filter.hpp
    low_pass_filter_creator.cpp low_pass_filter_creator.hpp
    math_clamp.hpp
    math_compare_decimals.hpp
    math_map.hpp
    network/bond_deserializer.cpp network/bond_deserializer.hpp
    network/bond_parser.cpp network/bond_parser.hpp
    network/bond_sender.cpp network/bond_sender.hpp
    network/command_parser.cpp network/command_parser.hpp
    network/connector.cpp network/connector.hpp
    network/foreign_sensor/requester.cpp network/foreign_sensor/requester.hpp
    network/foreign_sensor/response.cpp network/foreign_sensor/response.hpp
    network/login/response.cpp network/login/response.hpp
    network/login/sender.cpp network/login/sender.hpp
    network/response/message.cpp network/response/message.hpp
    network/response/send_helper.cpp network/response/send_helper.hpp
    network/response/sender.cpp network/response/sender.hpp
    network/response/simple.cpp network/response/simple.hpp
    network/sensor_id_helper.cpp network/sensor_id_helper.hpp
    network/sensor_profile_helper.cpp network/sensor_profile_helper.hpp
    network/sensor_value_type_helper.cpp network/sensor_value_type_helper.hpp
    onboard_sensor_checker.cpp onboard_sensor_checker.hpp
    outdoor_sensor.cpp outdoor_sensor.hpp
    performance_profiler.cpp performance_profiler.hpp
    plug_n_play.cpp plug_n_play.hpp
    profile_cache.cpp profile_cache.hpp
    profile_handler.cpp profile_handler.hpp
    program_options_parser.cpp program_options_parser.hpp
    room.cpp room.hpp
    room_cache.cpp room_cache.hpp
    room_handler.cpp room_handler.hpp
    room_name_index_handler.cpp room_name_index_handler.hpp
    room_name_langanguage_changer.cpp room_name_langanguage_changer.hpp
    room_names.hpp
    scd30_handler.cpp scd30_handler.hpp
    scd30_parser.cpp scd30_parser.hpp
    scd30_reader.cpp scd30_reader.hpp
    sensirion/crc.cpp sensirion/crc.hpp
    sensirion/uint16_parser.cpp sensirion/uint16_parser.hpp
    sensor_filter.cpp sensor_filter.hpp
    sensor_id.cpp sensor_id.hpp
    sensor_id_comfort_questionnaire.cpp sensor_id_comfort_questionnaire.hpp
    sensor_id_ee895.cpp sensor_id_ee895.hpp
    sensor_id_enocean.cpp sensor_id_enocean.hpp
    sensor_id_rssi.cpp sensor_id_rssi.hpp
    sensor_id_scd30.cpp sensor_id_scd30.hpp
    sensor_id_sgpc3.cpp sensor_id_sgpc3.hpp
    sensor_id_sht25.cpp sensor_id_sht25.hpp
    sensor_id_sht85.cpp sensor_id_sht85.hpp
    sensor_id_shtc1.cpp sensor_id_shtc1.hpp
    sensor_id_tmp117.cpp sensor_id_tmp117.hpp
    sensor_metadata.cpp sensor_metadata.hpp
    sensor_metadata_creator.cpp sensor_metadata_creator.hpp
    sensor_profile.cpp sensor_profile.hpp
    sensor_type.cpp sensor_type.hpp
    sensor_type_helper.cpp sensor_type_helper.hpp
    sensor_value.cpp sensor_value.hpp
    sensor_value_handler.cpp sensor_value_handler.hpp
    sensor_value_offset.cpp sensor_value_offset.hpp
    sensor_value_offset_cache.cpp sensor_value_offset_cache.hpp
    sensor_value_offset_handler.cpp sensor_value_offset_handler.hpp
    sensor_value_type.cpp sensor_value_type.hpp
    sensor_value_type_helper.cpp sensor_value_type_helper.hpp
    serial_interface.cpp serial_interface.hpp
    sgpc3_handler.cpp sgpc3_handler.hpp
    sgpc3_parser.cpp sgpc3_parser.hpp
    sgpc3_reader.cpp sgpc3_reader.hpp
    sht25_handler.cpp sht25_handler.hpp
    sht25_parser.cpp sht25_parser.hpp
    sht25_reader.cpp sht25_reader.hpp
    sht85_handler.cpp sht85_handler.hpp
    sht85_parser.cpp sht85_parser.hpp
    sht85_reader.cpp sht85_reader.hpp
    shtc1_handler.cpp shtc1_handler.hpp
    shtc1_parser.cpp shtc1_parser.hpp
    shtc1_reader.cpp shtc1_reader.hpp
    std_array_printer.cpp std_array_printer.hpp
    sync_state.cpp sync_state.hpp
    table_sync_helper.cpp table_sync_helper.hpp
    thread_checker.cpp thread_checker.hpp
    timestamp_helper.cpp timestamp_helper.hpp
    tmp117_handler.cpp tmp117_handler.hpp
    tmp117_parser.cpp tmp117_parser.hpp
    tmp117_reader.cpp tmp117_reader.hpp
    tone_writer.cpp tone_writer.hpp
    types.cpp types.hpp
    url_helper.cpp url_helper.hpp
    uuid_generator.cpp uuid_generator.hpp
    uuid_helper.cpp uuid_helper.hpp
    value_smoother.cpp value_smoother.hpp
    websocket_config.cpp websocket_config.hpp
    websocket_logger.cpp websocket_logger.hpp
    websocket_sync.cpp websocket_sync.hpp
)

set(GENERATED_SOURCE_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_reflection.h
    ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_apply.cpp ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_apply.h
    ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_types.cpp ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_types.h
)

set_source_files_properties(${GENERATED_SOURCE_FILES}
                            PROPERTIES GENERATED TRUE)

add_custom_command(OUTPUT ${GENERATED_SOURCE_FILES}
    COMMAND ${BOND_GBC} cpp ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/mold_protocol.bond ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/mold_protocol_enums.bond
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wolf
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/mold_protocol.bond)

if(WOLF_ENABLE_GZIP)
    set(SOURCE_FILES ${SOURCE_FILES}
        gzip.cpp gzip.hpp)
endif(WOLF_ENABLE_GZIP)

add_library(wolf 
    ${SOURCE_FILES} 
    ${GENERATED_SOURCE_FILES}
)

target_link_libraries(wolf PUBLIC 
    log
    WiringPi::WiringPi
    Enocean::EoLink
    WebsocketPP::WebsocketPP
    Boost::program_options
    Boost::serialization
    Boost::system
    Boost::filesystem
    ${SOCI_LIBRARY}
    OpenSSL::SSL
    wolf::asio
    wolf::stacktrace
)
target_include_directories(wolf PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}/wolf
)
target_include_directories(wolf SYSTEM PUBLIC 
    ${WOLF_ALL_INCLUDES}
)

if (BUILD_SHARED_LIBS)
  install(TARGETS wolf
      LIBRARY DESTINATION lib
      )
endif ()

set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_apply.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/wolf/mold_protocol_types.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-return-type -Wno-strict-aliasing")
