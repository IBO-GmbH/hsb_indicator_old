#include "wifi_handler.h"

Wifi_handler::Wifi_handler() : m_tag("wifi_handler") {}

Wifi_handler::~Wifi_handler() {
  esp_err_t ret = deinit();
  if (ret != ESP_OK && ret != ESP_ERR_WIFI_NOT_INIT) {
    ESP_ERROR_CHECK(ret);
  }
}

void Wifi_handler::event_handler(void *arg, const esp_event_base_t event_base,
                                 const int32_t event_id,
                                 const void *event_dat) {
  ESP_LOGI(m_tag.c_str(), "WIFI HANDLER CALLED %ld", event_id);
  if (event_id == WIFI_EVENT_STA_START) {
    ESP_LOGI(m_tag.c_str(), "WIFI CONNECTING....\n");
    m_connecting = true;
  } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
    ESP_LOGI(m_tag.c_str(), "WiFi CONNECTED\n");
    m_connecting = false;
  } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(m_tag.c_str(), "WiFi lost connection\n");
    if (m_retry_num < MAXIMUM_WIFI_CONNECTION_RETRIES) {
      esp_wifi_connect();
      m_retry_num++;
      ESP_LOGI(m_tag.c_str(), "Retrying to Connect...\n");
    }
  } else if (event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(m_tag.c_str(), "Wifi got IP...\n\n");
  }
}

void Wifi_handler::event_handler_static(void *arg,
                                        const esp_event_base_t event_base,
                                        const int32_t event_id,
                                        void *event_data) {
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

  // check if default event loop already exists and if not create it
  esp_err_t default_event_loop_err = esp_event_loop_create_default();
  if (default_event_loop_err == ESP_ERR_INVALID_STATE) {
    // Default event loop already exists
    ESP_LOGI(m_tag.c_str(), "Default event loop already created.");
  } else {
    ESP_ERROR_CHECK(default_event_loop_err);
  }

  m_wifi_netif = esp_netif_create_default_wifi_sta();
  assert(m_wifi_netif);

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  //  add wifi event handlers
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_handler::event_handler_static, this));
  ESP_ERROR_CHECK(
      esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                 &Wifi_handler::event_handler_static, this));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

#if defined INIT_WIFI_SSID && defined INIT_WIFI_PASSWORD
  set_ssid_and_pw(INIT_WIFI_SSID, INIT_WIFI_PASSWORD);
#endif

  return ESP_OK;
}

esp_err_t Wifi_handler::deinit() {
  esp_err_t ret = esp_wifi_disconnect();
  if (ret != ESP_OK) {
    return ret;
  }
  ret = esp_wifi_stop();
  if (ret != ESP_OK) {
    return ret;
  }
  ret = esp_wifi_deinit();
  if (ret != ESP_OK) {
    return ret;
  }

  esp_netif_destroy(m_wifi_netif);
  m_wifi_netif = NULL;
  return ret;
}

esp_err_t Wifi_handler::connect_to_wifi() const {
  ESP_ERROR_CHECK(esp_wifi_disconnect());
  return esp_wifi_connect();
}

esp_err_t Wifi_handler::connect_to_wifi(const std::string_view ssid,
                                        const std::string_view password) {
  ESP_ERROR_CHECK(esp_wifi_disconnect());
  set_ssid_and_pw(ssid, password);
  return esp_wifi_connect();
}

esp_err_t Wifi_handler::set_ssid_and_pw(const std::string_view ssid,
                                        const std::string_view password) {
  if (ssid.length() > 32 - 1 || password.length() > 64 - 1) {
    ESP_LOGE(m_tag.c_str(), "Given SSID or password too long");
    assert(false);
  }
  wifi_config_t wifi_config;
  memset(&wifi_config, '\0', sizeof(wifi_config_t));
  memcpy((char *)wifi_config.sta.ssid, ssid.data(), ssid.length());
  memcpy((char *)wifi_config.sta.password, password.data(), password.length());

  ESP_LOGI(m_tag.c_str(), "Wifi config: SSID=%.*s, PASS=%.*s",
           static_cast<int>(ssid.length()), ssid.data(),
           static_cast<int>(password.length()), password.data());
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  m_ssid = ssid;
  m_password = password;
  return ESP_OK;
}

esp_err_t Wifi_handler::scan(std::span<wifi_ap_record_t> ap_info) const {
  if (m_connecting) {
    return ESP_ERR_INVALID_STATE;
  }
  uint16_t number_of_records = ap_info.size();
  uint16_t ap_count = 0;
  memset(&ap_info[0], 0, sizeof(ap_info));

#ifdef USE_CHANNEL_BTIMAP
  wifi_scan_config_t *scan_config =
      (wifi_scan_config_t *)calloc(1, sizeof(wifi_scan_config_t));
  if (!scan_config) {
    ESP_LOGE(m_tag.c_str(), "Memory Allocation for scan config failed!");
    return;
  }
  array_2_channel_bitmap(channel_list, CHANNEL_LIST_SIZE, scan_config);
  esp_wifi_scan_start(scan_config, true);
  free(scan_config);

#else
  // active blocking scan
  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
#endif  // USE_CHANNEL_BTIMAP

  ESP_LOGI(m_tag.c_str(), "Max AP number ap_info can hold = %u",
           number_of_records);
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
  ESP_ERROR_CHECK(
      esp_wifi_scan_get_ap_records(&number_of_records, &ap_info[0]));
  ESP_LOGI(m_tag.c_str(),
           "Total APs scanned = %u, actual AP number ap_info holds = %u",
           ap_count, number_of_records);
  return ESP_OK;
}

void Wifi_handler::log_ap_info(
    const std::span<wifi_ap_record_t> ap_info) const {
  for (std::size_t record_index = 0; record_index < ap_info.size();
       record_index++) {
    ESP_LOGI(m_tag.c_str(), "SSID \t\t%s", ap_info[record_index].ssid);
    ESP_LOGI(m_tag.c_str(), "RSSI \t\t%d", ap_info[record_index].rssi);
    ESP_LOGI(m_tag.c_str(), "Channel \t\t%d", ap_info[record_index].primary);
  }
}

bool Wifi_handler::ssid_in_records(const std::span<wifi_ap_record_t> ap_info,
                                   const std::string_view ssid) const {
  for (std::size_t record_index = 0; record_index < ap_info.size();
       ++record_index) {
    // Compare the ssid with the ssid in ap_info[i]
    // std::strncmp is used to compare fixed-size C-strings since string_view
    // does not have to end with a null terminator
    if (std::strncmp(reinterpret_cast<const char *>(ap_info[record_index].ssid),
                     ssid.data(), ssid.size()) == 0) {
      // Additional check to ensure the lengths match exactly
      if (ssid.size() == std::strlen(reinterpret_cast<const char *>(
                             ap_info[record_index].ssid))) {
        return true;
      }
    }
  }
  return false;
}

#ifdef USE_CHANNEL_BTIMAP
static void Wifi_handler::array_2_channel_bitmap(
    const uint8_t channel_list[], const uint8_t channel_list_size,
    wifi_scan_config_t *scan_config) {
  for (uint8_t channel_index = 0; channel_index < channel_list_size;
       channel_index++) {
    uint8_t channel = channel_list[channel_index];
    scan_config->channel_bitmap.ghz_2_channels |= (1 << channel);
  }
}
#endif  // USE_CHANNEL_BTIMAP