#include "wifi_handler.h"

Wifi_handler::Wifi_handler() : tag("wifi_handler") {}

Wifi_handler::~Wifi_handler() = default;

void Wifi_handler::event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_dat) {
  ESP_LOGI(tag.c_str(), "WIFI HANDLER CALLED", event_id, "\n");
  if (event_id == WIFI_EVENT_STA_START) {
    ESP_LOGI(tag.c_str(), "WIFI CONNECTING....\n");
  } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
    ESP_LOGI(tag.c_str(), "WiFi CONNECTED\n");
  } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(tag.c_str(), "WiFi lost connection\n");
    if (m_retry_num < 5) {
      esp_wifi_connect();
      m_retry_num++;
      ESP_LOGI(tag.c_str(), "Retrying to Connect...\n");
    }
  } else if (event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(tag.c_str(), "Wifi got IP...\n\n");
  }
}

void Wifi_handler::event_handler_static(void *arg, esp_event_base_t event_base,
                                        int32_t event_id, void *event_data) {
  Wifi_handler *instance = static_cast<Wifi_handler *>(arg);
  if (instance) {
    instance->event_handler(arg, event_base, event_id, event_data);
  } else {
    ESP_LOGE("event_handler_static",
             "Wifi did not get class from arg pointer. aborting\n");
    assert(false);
  }
}

esp_err_t Wifi_handler::init_sta() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  //  add wifi event handlers
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_handler::event_handler_static, this));
  ESP_ERROR_CHECK(
      esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                 &Wifi_handler::event_handler_static, this));
  // ESP_ERROR_CHECK(esp_event_handler_register(
  //     SC_EVENT, ESP_EVENT_ANY_ID, &Wifi_handler::event_handler_static,
  //     this));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  return ESP_OK;
}

esp_err_t Wifi_handler::connect_to_wifi(std::string ssid,
                                        std::string password) {
  // ESP_ERROR_CHECK(esp_wifi_disconnect());
  //  this->set_ssid_and_pw(ssid, password);
  //  esp_wifi_connect();
  //  return ESP_OK;
}

esp_err_t Wifi_handler::set_ssid_and_pw(std::string ssid,
                                        std::string password) {
  wifi_config_t wifi_config = {.sta =
                                   {
                                       .ssid = "",
                                       .password = "",
                                   }

  };
  strcpy((char *)wifi_config.sta.ssid, ssid.c_str());
  strcpy((char *)wifi_config.sta.password, password.c_str());
  ESP_LOGI(tag.c_str(), "Kconfig", "SSID=%s, PASS=%s", ssid, password);
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  m_ssid = ssid;
  m_password = password;
  return ESP_OK;
}

esp_err_t Wifi_handler::scan(std::span<wifi_ap_record_t> ap_info) {
  uint16_t number = ap_info.size();
  // wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
  uint16_t ap_count = 0;
  memset(&ap_info[0], 0, sizeof(ap_info));

#ifdef USE_CHANNEL_BTIMAP
  wifi_scan_config_t *scan_config =
      (wifi_scan_config_t *)calloc(1, sizeof(wifi_scan_config_t));
  if (!scan_config) {
    ESP_LOGE(tag.c_str(), "Memory Allocation for scan config failed!");
    return;
  }
  array_2_channel_bitmap(channel_list, CHANNEL_LIST_SIZE, scan_config);
  esp_wifi_scan_start(scan_config, true);
  free(scan_config);

#else
  // active blocking scan
  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
#endif  // USE_CHANNEL_BTIMAP

  ESP_LOGI(tag.c_str(), "Max AP number ap_info can hold = %u", number);
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, &ap_info[0]));
  ESP_LOGI(tag.c_str(),
           "Total APs scanned = %u, actual AP number ap_info holds = %u",
           ap_count, number);
  return ESP_OK;
}

void Wifi_handler::log_ap_info(std::span<wifi_ap_record_t> ap_info) {
  uint16_t number = ap_info.size();
  for (int i = 0; i < number; i++) {
    ESP_LOGI(tag.c_str(), "SSID \t\t%s", ap_info[i].ssid);
    ESP_LOGI(tag.c_str(), "RSSI \t\t%d", ap_info[i].rssi);
    // print_auth_mode(ap_info[i].authmode);
    if (ap_info[i].authmode != WIFI_AUTH_WEP) {
      // print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
    }
    ESP_LOGI(tag.c_str(), "Channel \t\t%d", ap_info[i].primary);
  }
}

#ifdef USE_CHANNEL_BTIMAP
static void Wifi_handler::array_2_channel_bitmap(
    const uint8_t channel_list[], const uint8_t channel_list_size,
    wifi_scan_config_t *scan_config) {
  for (uint8_t i = 0; i < channel_list_size; i++) {
    uint8_t channel = channel_list[i];
    scan_config->channel_bitmap.ghz_2_channels |= (1 << channel);
  }
}
#endif  // USE_CHANNEL_BTIMAP