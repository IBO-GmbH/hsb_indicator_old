#include "flash_controller.h"

Flash_controller::Flash_controller() : m_tag("flash_controller") {}

Flash_controller::~Flash_controller() = default;

esp_err_t Flash_controller::init() {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  return ret;
}
