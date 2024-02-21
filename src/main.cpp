#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include <assert.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "WebServer/WebServer.h"
#include "DataFilesManger/DataFilesManager.h"

WebServer webServer;
DataFilesManager DataFilesManager("/data-files");
JsonDocument doc;

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
  String ledState = DataFilesManager.load("led");

  if (ledState == "") {
    DataFilesManager.save("led", "0");
    ledState = "0";
  }

  Serial.println("ledState: " + ledState);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState.toInt() ? LOW : HIGH);
}

void handleRoot() {
  Serial.println("GET /api");
  webServer.send(200, "text/json", "{message: 'hello from esp8266!', success}");
}

void handleToggleLedState() {
  String ledState = DataFilesManager.load("led");
  bool newLedState = !ledState.toInt();

  DataFilesManager.save("led", String(newLedState));
  
  digitalWrite(LED_BUILTIN, newLedState ? LOW : HIGH);

  webServer.send(200, "text/json", "{\"state\": " + String(newLedState) + "}");
}

void handleGetLedState() {
  String ledState = DataFilesManager.load("led");

  webServer.send(200, "text/json", "{\"state\": " + ledState + "}");
}

void setup() {
  Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);

  while (!Serial) {
    delay(50);
  }
  
  DataFilesManager.begin();
  setupLed();
  connectToWifi(SECRET_SSID, SECRET_PASSWORD);
  webServer.begin();


  webServer.on("/api", HTTP_GET, handleRoot);
  webServer.on("/api/led/toggle", HTTP_GET, handleToggleLedState);
  webServer.on("/api/led", HTTP_GET, handleGetLedState);
}

void loop() {
  webServer.handleClient();
}
