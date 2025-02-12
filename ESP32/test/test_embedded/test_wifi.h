#ifndef TEST_WIFI_H
#define TEST_WIFI_H

#pragma once

#include "gtest/gtest.h"

//
#include "flash_manager.h"
#include "wifi_manager.h"

int test_wifi(int argc, char **argv);

class WifiTest : public testing::Test {
 protected:
  // functions

  WifiTest() {
    // You can do set-up work for each test here.
  }
  ~WifiTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }
  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
    m_wifi_manager.init_sta();
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).

    // wifi deinit handle implicitly by the wifi_handler destructor
    //  m_wifi_handler.deinit();
  }
  // variables
  //  init with default constructor
  Wifi_manager m_wifi_manager;
};

#endif  // TEST_WIFI_H