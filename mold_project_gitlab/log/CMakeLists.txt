add_library(log
 config.cpp config.hpp
 journal_sink.cpp journal_sink.hpp
 log.cpp log.hpp
 logger.cpp logger.hpp
 pragma.cpp pragma.hpp
 program_options.cpp program_options.hpp
 severity.cpp severity.hpp
)

find_library(LIBSYSTEMD_LIBRARY NAMES systemd)
find_path(LIBSYSTEMD_INCLUDE_DIR NAMES systemd/sd-journal.h)
add_library(systemd INTERFACE)
target_include_directories(systemd INTERFACE ${LIBSYSTEMD_INCLUDE_DIR})
target_link_libraries(systemd INTERFACE ${LIBSYSTEMD_LIBRARY})
add_library(Systemd::systemd ALIAS systemd)

target_link_libraries(log PUBLIC
    Boost::log
    Boost::system
    Boost::program_options
)
target_link_libraries(log PRIVATE
    Systemd::systemd
)
target_include_directories(log PUBLIC
    ..
)

install(TARGETS log
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)
