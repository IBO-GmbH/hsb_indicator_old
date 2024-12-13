
add_executable(enocean_serial_test serial_test.cpp)
target_link_libraries(enocean_serial_test
    wolf mold log
)

install(TARGETS enocean_serial_test
    RUNTIME DESTINATION bin
    )

add_executable(enocean_link_test link_test.cpp)
target_link_libraries(enocean_link_test
    Enocean::EoLink
)
install(TARGETS enocean_link_test
    RUNTIME DESTINATION bin
    )
