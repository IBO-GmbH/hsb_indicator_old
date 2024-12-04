#ifndef SNTP_SERVICE_H
#define SNTP_SERVICE_H

#pragma once

#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "time.h"

class Sntp_service {
 public:
  Sntp_service();
  ~Sntp_service();

  esp_err_t init() const;
  esp_err_t deinit() const;
};

#endif  // SNTP_SERVICE_H