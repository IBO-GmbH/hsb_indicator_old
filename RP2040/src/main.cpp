#include <Arduino.h>
#include <PacketSerial.h>
#include <Wire.h>

/******************************** Defines ********************************/

#define DEBUG 1
#define TRX_BUFFER_SIZE 32

#define PKT_TYPE_CMD_COLLECT_INTERVAL 0xA0
#define PKT_TYPE_CMD_BEEP_ON 0xA1
#define PKT_TYPE_CMD_SHUTDOWN 0xA3

/******************************* Variables *******************************/

PacketSerial myPacketSerial;

/*************************** Helper Functions ****************************/

// send sensor data to esp32
void sensor_data_send(uint8_t type, const std::vector<float>& data) {
  uint8_t data_buf[TRX_BUFFER_SIZE] = {0};
  int size = 0;

  data_buf[0] = type;
  size++;

  for (const auto& data_single : data) {
    memcpy(&data_buf[size], &data_single, sizeof(float));
    size += sizeof(float);
    if (size > TRX_BUFFER_SIZE) {
      Serial.println("Tried to send too many bytes!");
      return;
    }
  }

  myPacketSerial.send(data_buf, size);
#if DEBUG
  Serial.printf("---> send len:%d, data: ", size);
  for (int i = 0; i < size; i++) {
    Serial.printf("0x%x ", data_buf[i]);
  }
  Serial.println("");
#endif
}

void printUint16Hex(uint16_t value) {
  Serial.print(value < 4096 ? "0" : "");
  Serial.print(value < 256 ? "0" : "");
  Serial.print(value < 16 ? "0" : "");
  Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
  Serial.print("Serial: 0x");
  printUint16Hex(serial0);
  printUint16Hex(serial1);
  printUint16Hex(serial2);
  Serial.println();
}

void sensor_power_on(void) {
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
}

void sensor_power_off(void) {
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
}

/********************************* beep **********************************/

#define Buzzer 19  // Buzzer GPIO

void beep_init(void) { pinMode(Buzzer, OUTPUT); }
void beep_off(void) { digitalWrite(19, LOW); }
void beep_on(void) {
  analogWrite(Buzzer, 127);
  delay(50);
  analogWrite(Buzzer, 0);
}

/************************** recv cmd from esp32 **************************/

static bool shutdown_flag = false;

void onPacketReceived(const uint8_t* buffer, size_t size) {
#if DEBUG
  Serial.printf("<--- recv len:%d, data: ", size);
  for (int i = 0; i < size; i++) {
    Serial.printf("0x%x ", buffer[i]);
  }
  Serial.println("");
#endif
  if (size < 1) {
    return;
  }
  switch (buffer[0]) {
    case PKT_TYPE_CMD_SHUTDOWN: {
      Serial.println("cmd shutdown");
      shutdown_flag = true;
      sensor_power_off();
      break;
    }
    default:
      break;
  }
}

/***************************** setup & loop ******************************/

void setup() {
  Serial.begin(115200);

  Serial1.setRX(17);
  Serial1.setTX(16);
  Serial1.begin(115200);
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler(&onPacketReceived);

  sensor_power_on();

  Wire.setSDA(20);
  Wire.setSCL(21);
  Wire.begin();

  beep_init();
  delay(500);
  beep_off();
}

void loop() {
  Serial.println("Print test");
  delay(1000);
}
