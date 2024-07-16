#include <ui.h>

#include "bsp_board.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_port.h"

static const char *TAG = "app_main";

static void print_test_task(void *arg) {
  while (1) {
    ESP_LOGI(TAG, "Test print");
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

extern "C" void app_main() {
  ESP_ERROR_CHECK(bsp_board_init());
  lv_port_init();
  ui_init();
}