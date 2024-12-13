add_executable(sht25_test sht25_test.cpp)
target_link_libraries(sht25_test
    wolf mold
    ${LIBS}
)

install(TARGETS sht25_test
    RUNTIME DESTINATION bin
    )
