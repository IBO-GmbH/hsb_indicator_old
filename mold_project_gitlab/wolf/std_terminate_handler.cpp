#include "std_terminate_handler.hpp"
#include <boost/stacktrace.hpp>
#include <exception>
#include <iostream>

using namespace wolf;

std_terminate_handler::std_terminate_handler() {
  std::set_terminate([] {
    std::cerr << "std::terminate got called, stacktrace:"
              << boost::stacktrace::stacktrace() << std::endl;
  });
}
