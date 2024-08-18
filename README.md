# Esp8266 Preact

Work in progress 🚧

## Description

This project is a simple template to start a new project with Preact, tailwind and Vite for the ESP8266, using PlatformIO.

## How to use

1. Clone this repository
2. Create a `secrets.h` file inside the `src` using the `secrets.h.example` as a template. This file will contain the WiFi credentials.
3. Omit the secret `SECRET_JWT` for disabling authentication.
4. Install the dependencies inside the `interface` folder with `yarn`
5. Run `yarn build` to build the interface
6. Build and upload the file system image with `pio run -t buildfs && pio run -t uploadfs`, or use the [PlatformIO IDE](https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/)
7. Build and upload the code with `pio run -t upload` or use the PlatformIO IDE.
8. Open the serial monitor to see the IP address of the ESP8266 and access it on your browser.

### Development

- Create a `interface/.env` file following the `interface/.env.example`
- Run `yarn dev` inside the `interface/` folder
- API requests in localhost will be proxied to the ESP during development.