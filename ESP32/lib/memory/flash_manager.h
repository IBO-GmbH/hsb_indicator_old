#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#pragma once

#include <cstring>
#include <string>

#include "nvs_flash.h"

class Flash_manager {
 public:
  Flash_manager();
  ~Flash_manager();

  esp_err_t init();

 private:
  const std::string m_tag;
};

#endif  // FLASH_CONTROLLER_H