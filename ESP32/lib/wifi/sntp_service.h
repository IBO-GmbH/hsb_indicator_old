#ifndef SNTP_SERVICE_H
#define SNTP_SERVICE_H

#pragma once

#include "time.h"

// #include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

class Sntp_service {
 public:
  Sntp_service();
  ~Sntp_service();

  esp_err_t init();
  esp_err_t deinit();

 private:
};

#endif  // SNTP_SERVICE_H