#ifndef SHT45_H
#define SHT45_H

#include <Arduino.h>
#include <SensirionErrors.h>
#include <SensirionI2cSht4x.h>
#include <Wire.h>

class Sht45 : public SensirionI2cSht4x {
 public:
  /**
   * \brief sht45 with init and read function
   */

  Sht45();
  void sensor_init();
  int16_t sensor_read(std::pair<float, float> &);
  int16_t sensor_read(float &, float &);

  uint8_t get_sensor_connected() const { return m_sensor_connected; }

 private:
  bool m_sensor_connected = false;
};

#endif  // SHT45_H