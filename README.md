# Esp8266 / Esp32 Preact

## Description

This project is a simple template to start a new project with Preact, tailwind and Vite for the ESP8266, using PlatformIO.

## How to use

1. Clone this repository
2. Create a `secrets.h` file inside the `src` using the `secrets.h.example` as a template. This file will contain the WiFi credentials.
3. Omit the secret `SECRET_JWT` for disabling authentication.
4. Build and upload the code with `pio run -t upload` or use the PlatformIO IDE.
5. Open the serial monitor to see the IP address of the ESP8266 and access it on your browser.

### ESP32

To use this project in ESP32 boards, just open the `platformio.ini` file, comment the line with `default_envs = esp12e` and uncomment the line with `default_envs = node32s`. Platformio should handle everything else by itself.

### Development

- Create a `interface/.env` file following the `interface/.env.example`
- Run `yarn dev` inside the `interface/` folder
- API requests in localhost will be proxied to the ESP during development.
