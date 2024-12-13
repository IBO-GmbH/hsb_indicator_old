find_path(Enocean_INCLUDE_DIR
    NAMES eoLink.h
)
find_library(Enocean_LIBRARY
    NAMES eolink
)

mark_as_advanced(Enocean_FOUND Enocean_INCLUDE_DIR Enocean_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Enocean
    REQUIRED_VARS Enocean_INCLUDE_DIR Enocean_LIBRARY
)

if(Enocean_FOUND)
    set(Enocean_INCLUDE_DIRS ${Enocean_INCLUDE_DIR} ${Enocean_INCLUDE_DIR}/Includes)
    set(Enocean_LIBRARIES ${Enocean_LIBRARY})
endif()

if(Enocean_FOUND AND NOT TARGET Enocean::EoLink)
    add_library(Enocean::EoLink INTERFACE IMPORTED)
    set_target_properties(Enocean::EoLink PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Enocean_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES "${Enocean_LIBRARIES}"
    )
endif()