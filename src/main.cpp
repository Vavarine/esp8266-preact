#include <Arduino.h>
#include "LittleFS.h"
#include <assert.h>
#include <ArduinoJson.h>

#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include "secrets.h"
#include "WebServer/WebServer.h"
#include "DataFilesManger/DataFilesManager.h"
#include "OTAManager/OTAManager.h"

WebServer webServer;
DataFilesManager dataFilesManager("/data-files");
OTAManager otaManager;

#define LED_PIN 2

// On ESP32, the built-in LED is on when the pin is HIGH
#ifdef ESP32
  #define LED_ON 0x1
  #define LED_OFF 0x0
#elif defined(ESP8266)
  #define LED_ON 0x0
  #define LED_OFF 0x1
#endif

void connectToWifi(const char *ssid, const char *password) {
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  #if SECRET_DISABLE_WIFI_SLEEP
    Serial.println("WARNING: Disabling WiFi sleep mode");
    WiFi.setSleep(false);
  #endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.print(WiFi.localIP());

  // saves the ip address to a file
  Serial.println("");
}

void setupLed() {
  String ledState = dataFilesManager.load("led");

  if (ledState == "") {
    dataFilesManager.save("led", "0");
    ledState = "0";
  }

  Serial.println("ledState: " + ledState);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState.toInt() ? LED_ON : LED_OFF);
}

void handleToggleLedState(AsyncWebServerRequest* request, User* user) {
  Serial.println("GET /api/led/toggle");
  Serial.println("Request from user: " + user->username);

  String ledState = dataFilesManager.load("led");
  bool newLedState = !ledState.toInt();

  dataFilesManager.save("led", String(newLedState));
  
  digitalWrite(LED_PIN, newLedState ? LED_ON : LED_OFF);

  request->send(200, "application/json", "{\"state\": " + String(newLedState) + "}");
}

void handleGetLedState(AsyncWebServerRequest* request) {
  String ledState = dataFilesManager.load("led");

  request->send(200, "application/json", "{\"state\": " + ledState + "}");
}

void handleRestart(AsyncWebServerRequest* request) {
  Serial.println("GET /api/restart");
  request->send(201);
  delay(500);
  WiFi.disconnect(true);
  delay(500);
  ESP.restart();
}

void setup() {
  #ifdef ESP32
    Serial.begin(9600);
  #elif defined(ESP8266)
    Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);
  #endif

  while (!Serial) {
    delay(50);
  }
  
  dataFilesManager.begin();
  setupLed();
  connectToWifi(SECRET_SSID, SECRET_PASSWORD);
  otaManager.begin();

  webServer.on("/api/led/toggle", HTTP_GET, REQUIRE_AUTH, handleToggleLedState);
  webServer.on("/api/led", HTTP_GET, handleGetLedState);
  webServer.on("/api/restart", HTTP_POST, REQUIRE_AUTH, handleRestart);

  webServer.begin();
}

void loop() {
  otaManager.loop();
}
