
add_executable(tone_test tone_test.cpp)
target_link_libraries(tone_test
    wolf mold log
    ${LIBS}
)
install(TARGETS tone_test
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    )

