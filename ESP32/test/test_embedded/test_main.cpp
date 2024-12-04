#include <gtest/gtest.h>

// external libaries
#include "bsp_board.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// soure libaries
#include "flash_controller.h"
// test libaries
#include "test_wifi.h"
// TEST(...)
// TEST_F(...)

static const char *TAG = "test_app_main";

TEST(DummyTest, ShouldPass) { EXPECT_EQ(1, 1); }

extern "C" void app_main() {
  // filter to select specific test cases
  ::testing::GTEST_FLAG(filter) =
      "*";  // e.g.: "DummyTest.ShouldPass", default = "*"
  ::testing::InitGoogleTest();
  ESP_ERROR_CHECK(bsp_board_init());
  Flash_controller memory;
  memory.init();
  // create default event loop for wifi
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  //
  if (RUN_ALL_TESTS()) {
  }

  // test_wifi(0, NULL);
}
