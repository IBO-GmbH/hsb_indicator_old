#ifndef ESP_LOG_H
#define ESP_LOG_H

#include <stdio.h>

// Define the log levels if they are used in your code
#define ESP_LOG_DEBUG 0
#define ESP_LOG_INFO 1
#define ESP_LOG_WARN 2
#define ESP_LOG_ERROR 3

// Mock the ESP-IDF logging functions
#define ESP_LOGE(tag, fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) printf("[WARN] " fmt "\n", ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)

#endif  // ESP_LOG_H