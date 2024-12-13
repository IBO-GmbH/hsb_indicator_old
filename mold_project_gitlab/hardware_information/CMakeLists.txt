add_executable(hardware_information hardware_test.cpp)

target_link_libraries(hardware_information
    wolf mold
    ${LIBS}
)

install(TARGETS hardware_information
    RUNTIME DESTINATION bin
    )
