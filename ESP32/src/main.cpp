#include <ui.h>

#include "bsp_board.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_port.h"
#include "time.h"

// config files
#ifdef __has_include
#if __has_include("credentials.h")
#include "credentials.h"
#endif
#endif
// source files
#include "flash_manager.h"
#include "sntp_service.h"
#include "wifi_manager.h"

#define READ_PRIORITY (configMAX_PRIORITIES - 1)

static const char *TAG = "app_main";

Wifi_manager wifi_manager;
Sntp_service sntp;

// TODO: enable when implemented
/*static void read_uart_task(void *arg) {
  while (1) {
    // read from RP2040
  }
}
*/

static void print_test_task(void *arg) {
  while (1) {
    ESP_LOGI(TAG, "Test print");
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void scan_wifi() {
  std::array<wifi_ap_record_t, (size_t)DEFAULT_SCAN_LIST_SIZE> scan_res;
  while (wifi_manager.scan(scan_res) != ESP_OK) {
    ESP_LOGI(TAG, "Scan failed. Wait and try again");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 s delay
  };
  wifi_manager.log_ap_info(scan_res);
}

extern "C" void app_main() {
  ESP_ERROR_CHECK(bsp_board_init());

  // init internal Flash
  Flash_manager memory;
  memory.init();

  // init display functionality
  lv_port_init();
  ui_init();

  // create default event loop for wifi
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // init WIFI and esablish a connection to the AP set in the credentials
  ESP_ERROR_CHECK(wifi_manager.init_sta());
#if defined INIT_WIFI_SSID && defined INIT_WIFI_PASSWORD
  ESP_ERROR_CHECK(
      wifi_manager.set_ssid_and_pw(INIT_WIFI_SSID, INIT_WIFI_PASSWORD));
  ESP_ERROR_CHECK(wifi_manager.connect_to_wifi());
#endif

  // start SNTP service
  ESP_ERROR_CHECK(sntp.init());
}