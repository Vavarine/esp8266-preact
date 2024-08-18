#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include <assert.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "WebServer/WebServer.h"
#include "DataFilesManger/DataFilesManager.h"
#include "OTAManager/OTAManager.h"

WebServer webServer;
DataFilesManager dataFilesManager("/data-files");
OTAManager otaManager;

// built-in LED
const int debugLed = D4;

void connectToWifi(const char *ssid, const char *password) {
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState.toInt() ? LOW : HIGH);
}

void handleRoot() {
  Serial.println("GET /api");
  webServer.send(200, "application/json", "{\"message\":\"hello from esp8266!\", \"success\":true}");
}

void handleToggleLedState(User* user) {
  Serial.println("GET /api/led/toggle");
  Serial.println("Request from user: " + user->username);

  String ledState = dataFilesManager.load("led");
  bool newLedState = !ledState.toInt();

  dataFilesManager.save("led", String(newLedState));
  
  digitalWrite(LED_BUILTIN, newLedState ? LOW : HIGH);

  webServer.send(200, "application/json", "{\"state\": " + String(newLedState) + "}");
}

void handleGetLedState() {
  String ledState = dataFilesManager.load("led");

  webServer.send(200, "application/json", "{\"state\": " + ledState + "}");
}

void handleRestart() {
  Serial.println("GET /api/restart");
  webServer.send(201);
  delay(500);
  WiFi.disconnect(true);
  delay(500);
  ESP.restart();
}

void setup() {
  Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);

  while (!Serial) {
    delay(50);
  }
  
  dataFilesManager.begin();
  setupLed();
  connectToWifi(SECRET_SSID, SECRET_PASSWORD);
  otaManager.begin();
  webServer.begin();

  webServer.on("/api", HTTP_GET, handleRoot);
  webServer.on("/api/led/toggle", HTTP_GET, REQUIRE_AUTH, handleToggleLedState);
  webServer.on("/api/led", HTTP_GET, handleGetLedState);
  webServer.on("/api/restart", HTTP_POST, REQUIRE_AUTH, handleRestart);
}

void loop() {
  otaManager.loop();
  webServer.handleClient();
}
