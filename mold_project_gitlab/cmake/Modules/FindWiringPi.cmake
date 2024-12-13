find_path(WiringPi_INCLUDE_DIR
    NAMES wiringPi.h
)
find_library(WiringPi_LIBRARY
    NAMES wiringPi
)

mark_as_advanced(WiringPi_FOUND WiringPi_INCLUDE_DIR WiringPi_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WiringPi
    REQUIRED_VARS WiringPi_INCLUDE_DIR WiringPi_LIBRARY
)

if(WiringPi_FOUND)
    set(WiringPi_INCLUDE_DIRS ${WiringPi_INCLUDE_DIR})
    set(WiringPi_LIBRARIES ${WiringPi_LIBRARY})
endif()

if(WiringPi_FOUND AND NOT TARGET WiringPi::WiringPi)
    add_library(WiringPi::WiringPi INTERFACE IMPORTED)
    set_target_properties(WiringPi::WiringPi PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${WiringPi_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${WiringPi_LIBRARIES}"
    )
endif()