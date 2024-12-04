# HSB Indicator

## Overview

HSB Indicator is a C++ project designed to be flashed onto a Seeed indicator, utilizing the internal ESP32 and RP2040 microcontrollers. This project is built to serve as the software for an indicator box that gathers data from various sensors, transmits it to the cloud, and displays the information on its screen. Additionally, users can configure settings and perform adjustments using a touchscreen interface.

## Features

- **I2C Sensors**: Collect data from connected sensors.
- **Value Communication**: Send data between the ESP32 and the RP2040
- **Interactive Display**: Show real-time data and statuses on the built-in display and configure settings easily through an intuitive touchscreen interface.
- **SD Card Storage**: Store sensor data directly on the internal SD card connected to the RP2040.
- **Wifi**: Use Wifi capabilities.
- **Bluetooth**: Use Bluetooth capabilities.
- **MQTT Broker**: Able to act as an MQTT broker.
- **LoRa Gateway**: Able to act as an LoRa gateway.

## Requirements

To build and run the HSB Indicator project, you will need:

- [PlatformIO](https://platformio.org/) installed in your development environment.
- A compatible Seeed indicator device with ESP32 and RP2040.
- Connected sensors compatible with the project requirements.

### Windows Drivers

To upload the code to the seeed indicator the correct drivers for the ESP32 and RP2040 have to be installed, if they are not installed automatically.

- ESP32

    - all 4 [CP210x driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads) (especially the CP210x_Windows_Drivers_with_Serial_Enumeration)

    - the [CH341SER driver](https://www.wch-ic.com/downloads/CH341SER_ZIP.html)
    


- RP2040

    Use [Zadig](https://zadig.akeo.ie/) to load WinUSB drivers for the "RP2 Boot2 (Interface 1)" device as described in this [issue](https://github.com/earlephilhower/arduino-pico/issues/520).


## Getting Started

Follow these steps to get started with the HSB Indicator project:

1. **Clone the Repository**: 
   ```bash
   git clone https://github.com/AndreasRichie/hsb_indicator.git
   cd hsb-indicator
    ```

2. **Open the project folder with Visual Studio Code with the PlatformIO extension**

3. **Switch to Default(ESP32) PlatformIO Project Enviroment**

4. **PlatformIO: Build**

5. **PlatformIO: Test**

6. **PlatformIO: Upload**

7. **Repeat steps 4-6 with the PlatformIO Project Enviroment Default(RP2040)**