
#include <sht45.h>

Sht45::Sht45() : SensirionI2cSht4x() {}

void Sht45::sensor_init() {
  char error_message[64];
  int16_t error;
#if DEBUG
  Serial.println(F("SHT45 Temperature and Humidity Sensor!"));
#endif

  this->begin(Wire, SHT40_I2C_ADDR_44);

  error = this->softReset();

  if (error != NoError) {
    Serial.print("Error trying to do soft reset: ");
    errorToString(error, error_message, sizeof error_message);
    Serial.println(error_message);
    m_sensor_connected = false;
    return;
  }
  delay(10);

  uint32_t serialNumber = 0;
  error = this->serialNumber(serialNumber);
  if (error != NoError) {
    Serial.print("Error trying to read serial number: ");
    errorToString(error, error_message, sizeof(error_message));
    Serial.println(error_message);
    m_sensor_connected = false;
    return;
  }

#if DEBUG
  Serial.print("serialNumber: ");
  Serial.print(serialNumber);
  Serial.println();
#endif

  m_sensor_connected = true;
  Serial.println("-- Air Temperature and Humidity Sensor Connected --");
}

int16_t Sht45::sensor_read(std::pair<float, float> &values) {
  return this->sensor_read(values.first, values.second);
}

int16_t Sht45::sensor_read(float &temperature, float &humidity) {
  char error_message[64];
  int16_t error;

  temperature = 0.0;
  humidity = 0.0;
  delay(20);
  error = this->measureHighPrecision(temperature, humidity);
  if (error != NoError) {
    Serial.print("Error trying to read sensor data: ");
    errorToString(error, error_message, sizeof(error_message));
    Serial.println(error_message);
    return error;
  }

#if DEBUG
  Serial.print("SHT45 Temperature: ");
  Serial.print(temperature);
  Serial.print(", Humidity: ");
  Serial.println(humidity);
#endif

  return error;
}
