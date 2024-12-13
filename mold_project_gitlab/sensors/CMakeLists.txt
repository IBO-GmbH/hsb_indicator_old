add_executable(sensors
    sensors.cpp
    sht25_test.cpp sht25_test.hpp
    shtc1_test.cpp shtc1_test.hpp
    sgpc3_test.cpp sgpc3_test.hpp
    scd30_test.cpp scd30_test.hpp
    ee895_test.cpp ee895_test.hpp
    sht85_test.cpp sht85_test.hpp
    tmp117_test.cpp tmp117_test.hpp
    )
target_link_libraries(sensors
    wolf mold
    ${LIBS}
)

install(TARGETS sensors
    RUNTIME DESTINATION bin
    )
