#include "test_wifi.h"

// TODO fix test case
// void test_wifi_default_connection(void) {
TEST_F(WifiTest, DefaultConnectionEstablish) {
  esp_err_t err_connection;

  err_connection = m_wifi_handler.connect_to_wifi();

  EXPECT_EQ(err_connection, ESP_OK)
      << "Wifi connection could not be established";
}

int test_wifi(int argc, char **argv) {
  ::testing::InitGoogleTest();
  if (RUN_ALL_TESTS()) {
  }
  return 0;
}