#include "test_wifi.h"

// TODO fix test case
// void test_wifi_default_connection(void) {
TEST(Wifi_test, ShouldPass) {
  Wifi_handler wifi_handler;
  esp_err_t err_connection;

  wifi_handler.init_sta();
  err_connection = wifi_handler.connect_to_wifi();

  EXPECT_EQ(err_connection, ESP_OK);
}

int test_wifi(int argc, char **argv) {
  ::testing::InitGoogleTest();
  if (RUN_ALL_TESTS()) {
  }
  return 0;
}