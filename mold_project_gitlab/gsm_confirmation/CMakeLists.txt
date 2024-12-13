
set (UI_FILES
    gui/ConfirmationForm.ui
)

# qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(UIS_HDRS
    ${UI_FILES}
)

set (SOURCE_FILES_CONFIRMATION_GUI
    gui/ConfirmationForm.cpp gui/ConfirmationForm.hpp
)

set (STYLESHEETS
    ${CMAKE_SOURCE_DIR}/mold/gui/stylesheets/stylesheet.qss
)

set (SOURCE_FILES_CONFIRMATION
    gsm_confirmation.cpp
    gsm_confirmation_file.cpp
    gsm_confirmation_file.hpp

# qt
    ${confirmation_translations}
    ${SOURCE_FILES_CONFIRMATION_GUI}
    ${STYLESHEETS}
    ${UI_FILES}
)

add_executable(gsm_confirmation ${SOURCE_FILES_CONFIRMATION})
target_link_libraries(gsm_confirmation
    Qt5::Core Qt5::Gui Qt5::Widgets
)

install(TARGETS gsm_confirmation
    RUNTIME DESTINATION bin
    )

install(FILES scripts/exec-gsm_confirmation
    DESTINATION .
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    )
