#include "test_wifi.h"

// void test_wifi_default_connection(void) {
TEST_F(WifiTest, DefaultConnectionEstablish) {
  // connect to wifi defined in the credentials
  EXPECT_EQ(m_wifi_handler.connect_to_wifi(), ESP_OK)
      << "Wifi connection could not be established";
}

TEST_F(WifiTest, Scan) {
  EXPECT_EQ(m_wifi_handler.connect_to_wifi(), ESP_OK)
      << "Wifi connection could not be established";

  int max_tries = 10;
  int current_try = 0;

  // create ap_records array in which the scan result is saved
  std::array<wifi_ap_record_t, (size_t)DEFAULT_SCAN_LIST_SIZE> scan_res;
  while (m_wifi_handler.scan(scan_res) != ESP_OK) {
    current_try++;
    ASSERT_LT(current_try, max_tries);
    std::cout << "Scan failed. Wait and try again" << std::endl;
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 s delay
  };

  // print scan result
  m_wifi_handler.log_ap_info(scan_res);

  // get connected AP and check if it is in the scan result
  wifi_ap_record_t connected_ap_info;
  esp_wifi_sta_get_ap_info(&connected_ap_info);
  bool found_own_ssid_in_scan = false;
  for (int scan_list_index = 0; scan_list_index < scan_res.size();
       scan_list_index++) {
    if (memcmp(connected_ap_info.ssid, scan_res[scan_list_index].ssid,
               sizeof(connected_ap_info.ssid)) == 0) {
      found_own_ssid_in_scan = true;
    }
  }
  // check if connected AP in in scan result
  ASSERT_TRUE(found_own_ssid_in_scan);
}
