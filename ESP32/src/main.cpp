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
#include "flash_controller.h"
#include "sntp_service.h"
//#include "uart_handler.h"
#include "wifi_handler.h"

#define READ_PRIORITY (configMAX_PRIORITIES - 1)

static const char *TAG = "app_main";

//uart_handler uart_handler_;
Wifi_handler wifi_handler;
Sntp_service sntp;

static void read_uart_task(void *arg) {
  while (1) {
    //uart_handler_.read_from_sensor();
  }
}

static void print_test_task(void *arg) {
  while (1) {
    ESP_LOGI(TAG, "Test print");
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

void scan_wifi() {
  std::array<wifi_ap_record_t, (size_t)DEFAULT_SCAN_LIST_SIZE> scan_res;
  while (wifi_handler.scan(scan_res) != ESP_OK) {
    ESP_LOGI(TAG, "Scan failed. Wait and try again");
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 s delay
  };
  wifi_handler.log_ap_info(scan_res);
}

extern "C" void app_main() {
  ESP_ERROR_CHECK(bsp_board_init());

  // init internal Flash
  Flash_controller memory;
  memory.init();

  // init display functionality
  lv_port_init();
  ui_init();

  // create default event loop for wifi
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // init WIFI and esablish a connection to the AP set in the credentials
  ESP_ERROR_CHECK(wifi_handler.init_sta());
  ESP_ERROR_CHECK(wifi_handler.connect_to_wifi());
  //ESP_LOGI(TAG, "SSID: %s", INIT_WIFI_SSID);

  // start SNTP service
  ESP_ERROR_CHECK(sntp.init());

  // uart_handler_.init_uart();

  // xTaskCreate(read_uart_task, "read_uart_task", 1024 * 6, NULL,
  // READ_PRIORITY, NULL);
  // lv_label_set_text_fmt(ui_temperatureLabel, "%.1f°C", 20.5);
}