#include "sntp_service.h"

Sntp_service::Sntp_service() {}

Sntp_service::~Sntp_service() {}

void Sntp_service::init() {
  setenv("TZ", "UTC0", 1);
  tzset();
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  esp_netif_sntp_init(&config);
}