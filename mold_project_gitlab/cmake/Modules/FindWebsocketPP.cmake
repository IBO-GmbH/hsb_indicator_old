find_path(WebsocketPP_INCLUDE_DIR
    NAMES websocketpp/version.hpp
)

mark_as_advanced(WebsocketPP_FOUND WebsocketPP_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WebsocketPP
    REQUIRED_VARS WebsocketPP_INCLUDE_DIR
)

if(WebsocketPP_FOUND)
    set(WebsocketPP_INCLUDE_DIRS ${WebsocketPP_INCLUDE_DIR})
endif()

if(WebsocketPP_FOUND AND NOT TARGET WebsocketPP::WebsocketPP)
    add_library(WebsocketPP::WebsocketPP INTERFACE IMPORTED)
    set_target_properties(WebsocketPP::WebsocketPP PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${WebsocketPP_INCLUDE_DIRS}"
    )
endif()