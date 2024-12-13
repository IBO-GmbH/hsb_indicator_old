add_library(tests_gtest
  #enocean_handler_test.cpp
  RoomNameHandler_test.cpp
  boost_process_test.cpp
  comfort_questionnaire_cache_test.cpp
  configuration_cache_test.cpp
  configuration_state_handler_test.cpp
  controller_cache_test.cpp
  controller_handler_test.cpp
  controller_ventilation_test.cpp
  database_in_memory.cpp database_in_memory.hpp
  esp3_parser_test.cpp
  evaluation_humidity_test.cpp
  fixture_controller_ventilation_mock.hpp fixture_controller_ventilation_mock.cpp
  fixture_value_handler_mock.hpp fixture_value_handler_mock.cpp
  initial_room_creator_test.cpp
  key_value_cache_test.cpp
  low_pass_filter_test.cpp
  median_calculator_test.cpp
  mold_sprout_sender_test.cpp
  nightmode_handler_test.cpp
  on_off_control_test.cpp
  options_cache_test.cpp
  plug_n_play_test.cpp
  profile_cache_test.cpp
  room_cache_test.cpp
  room_handler_test.cpp
  sensirion_crc_test.cpp
  sensor_id_from_uuid_and_types_test.cpp
  sensor_value_getter_test
  sensor_value_handler_test.cpp
  sensor_value_offset_test.cpp
  svas1_ventilation_test.cpp
  svas2_dehumidifier_test.cpp
  svas3_heating_test.cpp
  svas4_air_quality_test.cpp
  sync_mold_sprout_handler_test.cpp
  sync_room_settings_test.cpp
  test_sensors.cpp test_sensors.hpp
)
target_link_libraries(tests_gtest PUBLIC
  GTest::gtest
  mold
  gsm
  wolf::boost_di
)

add_executable(tests_gtest-exec main.cpp)
target_link_libraries(tests_gtest-exec
    PUBLIC tests_gtest
)
SET_TARGET_PROPERTIES(tests_gtest-exec PROPERTIES OUTPUT_NAME tests_gtest)
install(TARGETS tests_gtest-exec tests_gtest
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)




