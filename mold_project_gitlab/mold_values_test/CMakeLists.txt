add_executable(mold_values_test mold_values_test.cpp)

target_link_libraries(mold_values_test
    wolf 
    mold
    ${LIBS}
)

install(TARGETS mold_values_test
    RUNTIME DESTINATION bin
)

