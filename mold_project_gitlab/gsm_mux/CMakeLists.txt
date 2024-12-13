add_library(gsm
  at_read_write.cpp at_read_write.hpp
  at_reader.cpp at_reader.hpp
  at_writer.cpp at_writer.hpp
  command_handler.cpp command_handler.hpp
  gsmmux.h
  iccid_reader.cpp iccid_reader.hpp
  online_checker.cpp online_checker.hpp
  pppd_handler.cpp pppd_handler.hpp
  pppd_output_checker.cpp pppd_output_checker.hpp
  provider_handler.cpp provider_handler.hpp
  provider_list.cpp provider_list.hpp
  provider_writer.cpp provider_writer.hpp
  rssi_reader.cpp rssi_reader.hpp
  stats_reader.cpp stats_reader.hpp
  string_print_helper.cpp string_print_helper.hpp
  )
target_include_directories(gsm SYSTEM PUBLIC
  ${Boost_INCLUDE_DIRS})

target_link_libraries(gsm PUBLIC
  log
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  WiringPi::WiringPi
  Threads::Threads  
  rt
  )
if (USE_ASAN)
  target_link_libraries(gsm PUBLIC
      -pthread
    )
endif (USE_ASAN)

add_executable(gsm_mux
  gsm_mux.cpp gsm_mux.hpp
  options.cpp options.hpp
  config.hpp
  )
target_link_libraries(gsm_mux PRIVATE
  gsm
  )

install(TARGETS gsm gsm_mux
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
