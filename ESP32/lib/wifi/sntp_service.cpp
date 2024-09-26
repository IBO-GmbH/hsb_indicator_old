#include "sntp_service.h"

Sntp_service::Sntp_service() {}

Sntp_service::~Sntp_service() {
  esp_err_t ret;
  ret = this->deinit();
  ESP_ERROR_CHECK(ret);
}

esp_err_t Sntp_service::init() {
  esp_err_t ret;
  setenv("TZ", "UTC0", 1);
  tzset();
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  config.start = false;  // start SNTP service explicitly
  ret = esp_netif_sntp_init(&config);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = esp_netif_sntp_start();
  return ret;
}

esp_err_t Sntp_service::deinit() {
  esp_netif_sntp_deinit();
  return ESP_OK;
}