set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_executable(keyboard_test main.cpp ui.qrc)
target_link_libraries(keyboard_test
    PRIVATE log Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Quick
    )
install(TARGETS keyboard_test
    RUNTIME DESTINATION bin
    )
