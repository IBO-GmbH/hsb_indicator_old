add_library(stacktrace
 stacktrace.cpp
)
target_link_libraries(stacktrace PUBLIC
    Boost::system
    log
)
install(TARGETS stacktrace
    LIBRARY DESTINATION lib
)
#find_path(LIBBACKTRACE_INCLUDE_DIR NAMES backtrace.h)
#target_include_directories(stacktrace PUBLIC "${LIBBACKTRACE_INCLUDE_DIR}")
#find_library(LIBBACKTRACE_LIBRARY NAMES backtrace)
#target_link_libraries(stacktrace PUBLIC "${LIBBACKTRACE_LIBRARY}")
target_compile_definitions(stacktrace PUBLIC
    BOOST_ENABLE_ASSERT_DEBUG_HANDLER
#    BOOST_STACKTRACE_USE_BACKTRACE
)
#target_compile_options(stacktrace PRIVATE)
add_library(wolf::stacktrace ALIAS stacktrace)
