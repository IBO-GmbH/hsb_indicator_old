set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(BUILD_SHARED_LIBS ON)

set(triple arm-linux-gnueabihf)

set(CMAKE_C_COMPILER "{compiler_dir}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER "{compiler_dir}/bin/arm-linux-gnueabihf-g++")
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

get_filename_component(BEAUTIFUL_SYSROOT "{sysroot}" ABSOLUTE)
set(CMAKE_SYSROOT "${BEAUTIFUL_SYSROOT}")
# ensure path is "beautiful and simple". don't add something like "<path>/./<more-path>". 
# else it won't find stdlib.h because of "-isystem" issues
# https://gitlab.kitware.com/cmake/cmake/issues/16291
# https://cmake.org/cmake/help/latest/prop_tgt/NO_SYSTEM_FROM_IMPORTED.html
# https://gcc.gnu.org/onlinedocs/gcc/Directory-Options.html
# https://stackoverflow.com/questions/52377652/i-and-isystem-different-behaviour-when-compiling
set(CMAKE_FIND_ROOT_PATH "${BEAUTIFUL_SYSROOT}" {dependencies_dir})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(qt_dir /qt5/lib/cmake/Qt5)
set(boost_dir /boost)
set(boost_di_dir /boost_di)
set(eolink_dir /eolink_1.9.0)
set(soci_dir /soci-3.2.3-install-linaro-arm)
set(websocketpp_dir /websocketpp-0.8.1)
set(bond_dir /{dependencies_dir}/bond)
set(wiringpi_dir /wiringpi)
set(libbacktrace_dir /libbacktrace)
set(googletest_dir /googletest)

set(CMAKE_PREFIX_PATH 
    ${qt_dir} 
    ${boost_dir}
    ${boost_di_dir}
    ${eolink_dir}
    ${soci_dir}
    ${websocketpp_dir}
    ${wiringpi_dir}
    ${libbacktrace_dir}
    ${googletest_dir}
    )
set(BOND_DIR ${bond_dir})
set(Boost_ARCHITECTURE "-a32")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf/ -Wl,-rpath-link,${CMAKE_SYSROOT}/lib/arm-linux-gnueabihf/")
