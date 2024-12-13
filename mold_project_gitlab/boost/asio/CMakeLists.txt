add_library(asio
 asio.cpp
)
target_link_libraries(asio PUBLIC
    Boost::system
)
install(TARGETS asio
    LIBRARY DESTINATION lib
)
target_compile_definitions(asio PUBLIC
    BOOST_ASIO_SEPARATE_COMPILATION
)
add_library(wolf::asio ALIAS asio)
