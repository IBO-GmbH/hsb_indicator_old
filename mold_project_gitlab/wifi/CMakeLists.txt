set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_library(wifi
    access_point.cpp access_point.hpp
    access_point_getter.cpp access_point_getter.hpp
    active_access_point_getter.cpp active_access_point_getter.hpp
    array_of_string_string.cpp array_of_string_string.hpp
    array_of_string_uint32.cpp array_of_string_uint32.hpp
    array_of_string_variant.cpp array_of_string_variant.hpp
    connection.cpp connection.hpp
    connector.cpp connector.hpp
    dbus_error.cpp dbus_error.hpp
    dbus_helper.cpp dbus_helper.hpp
    device_finder.cpp device_finder.hpp
    device_state.cpp device_state.hpp
    device_state_watcher.cpp device_state_watcher.hpp
    org_desktop_networkmanager.cpp org_desktop_networkmanager.h
    org_desktop_networkmanager_device.cpp org_desktop_networkmanager_device.h
    org_desktop_networkmanager_device_accesspoint.cpp org_desktop_networkmanager_device_accesspoint.h
    org_desktop_networkmanager_device_wireless.cpp org_desktop_networkmanager_device_wireless.h
    org_desktop_networkmanager_fwd.hpp
    rescanner.cpp rescanner.hpp
    scanner.cpp scanner.hpp
    wpa_supplicant_files/includes.hpp
    wpa_supplicant_files/build_config.hpp
    wpa_supplicant_files/common.hpp
    wpa_supplicant_files/os.hpp
    wpa_supplicant_files/wpa_ctrl.hpp wpa_supplicant_files/wpa_ctrl.cpp
    wpa_supplicant_files/wpa_ctrl_handler.hpp wpa_supplicant_files/wpa_ctrl_handler.cpp
    )
target_link_libraries(wifi
    PUBLIC Qt5::Core log
    PRIVATE Qt5::DBus
)

add_executable(wifi_scanner_test
    scanner_test.cpp
    )
target_link_libraries(wifi_scanner_test
    PUBLIC wifi
    )

add_executable(wifi_connector_test
    connector_test.cpp
    )
target_link_libraries(wifi_connector_test
    PUBLIC wifi
    )

add_executable(wpa_supplicant_test
    wpa_supplicant_test.cpp
    )
target_link_libraries(wpa_supplicant_test
    PUBLIC wifi
    )

install(TARGETS wifi wifi_scanner_test wifi_connector_test wpa_supplicant_test
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    )
