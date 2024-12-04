#ifndef FLASH_CONTROLLER_H
#define FLASH_CONTROLLER_H

#pragma once

#include <cstring>
#include <string>

#include "nvs_flash.h"

class Flash_controller {
 public:
  Flash_controller();
  ~Flash_controller();

  esp_err_t init();

 private:
  const std::string m_tag;
};

#endif  // FLASH_CONTROLLER_H