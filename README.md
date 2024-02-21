# Esp8266 Preact

Work in progress 🚧

## Description

This project is a simple template to start a new project with Preact and Webpack for the ESP8266, using PlatformIO.

## How to use

1. Clone this repository
2. Create a `secrets.h` file inside the `src` using the `secrets.h.example` as a template. This file will contain the WiFi credentials.
3. Install the dependencies inside the `interface` folder with `yarn`
4. Run `yarn build` to build the interface
5. Build and upload the file system image with `pio run -t buildfs` and `pio run -t uploadfs`, or use the [PlatformIO IDE](https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/)
6. Build and upload the code with `pio run -t upload` or use the PlatformIO IDE.
7. Open the serial monitor to see the IP address of the ESP8266 and access it on your browser.