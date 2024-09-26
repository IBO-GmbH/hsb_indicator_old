#include "test_wifi.h"

// void test_wifi_default_connection(void) {
TEST_F(WifiTest, DefaultConnectionEstablish) {
  esp_err_t err_connection;

  err_connection = m_wifi_handler.connect_to_wifi();

  EXPECT_EQ(err_connection, ESP_OK)
      << "Wifi connection could not be established";
}

TEST_F(WifiTest, Scan) {
  esp_err_t err_connection;

  err_connection = m_wifi_handler.connect_to_wifi();

  EXPECT_EQ(err_connection, ESP_OK)
      << "Wifi connection could not be established";

  int max_tries = 10;
  int current_try = 0;
  std::array<wifi_ap_record_t, (size_t)DEFAULT_SCAN_LIST_SIZE> scan_res;
  while (m_wifi_handler.scan(scan_res) != ESP_OK) {
    current_try = current_try + 1;
    ASSERT_LT(current_try, max_tries);
    std::cout << "Scan failed. Wait and try again" << std::endl;
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 s delay
  };
  m_wifi_handler.log_ap_info(scan_res);

  wifi_ap_record_t ap_info;
  esp_wifi_sta_get_ap_info(&ap_info);

  bool found_own_ssid_in_scan = false;
  for (int i = 0; i < scan_res.size(); i++) {
    if (memcmp(ap_info.ssid, scan_res[i].ssid, sizeof(ap_info.ssid)) == 0) {
      found_own_ssid_in_scan = true;
    }
  }
  // check if connected AP in in scan result
  ASSERT_TRUE(found_own_ssid_in_scan);
}

/*int test_wifi(int argc, char **argv) {
  ::testing::InitGoogleTest();
  if (RUN_ALL_TESTS()) {
  }
  return 0;
}*/