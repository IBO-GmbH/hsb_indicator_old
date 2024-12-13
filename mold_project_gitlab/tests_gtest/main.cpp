#include "gtest/gtest.h"
#include "log/log.hpp"
#include "wolf/thread_checker.hpp"

int main(int argc, char *argv[]) {
  wolf::thread_checker::register_as_service();
  logging::initialize();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
