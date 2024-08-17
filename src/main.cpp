#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "LittleFS.h"
#include <assert.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "WebServer/WebServer.h"
#include "DataFilesManger/DataFilesManager.h"
#include "OTAManager/OTAManager.h"
#include "ArduinoJsonJWT/ArduinoJsonJWT.h"

WebServer webServer;
DataFilesManager dataFilesManager("/data-files");
OTAManager otaManager;
ArduinoJsonJWT jwtHandler(SECRET_JWT);
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
  String ledState = dataFilesManager.load("led");

  if (ledState == "") {
    dataFilesManager.save("led", "0");
    ledState = "0";
  }

  Serial.println("ledState: " + ledState);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState.toInt() ? LOW : HIGH);
}

bool ensureAuthenticated() {
  String jwt = webServer.getCookie("esp.login");
  if (!jwt.length()) {
    webServer.send(401, "text/plain", "Unauthorized");
    jwt.clear();
    return false;
  }
  DynamicJsonDocument payloadDocument(MAX_JWT_SIZE);
  jwtHandler.parseJWT(jwt, payloadDocument);
  if (payloadDocument.isNull()) {
    webServer.send(403, "text/plain", "Forbidden");
    jwt.clear();
    return false;
  }
  jwt.clear();
  return true;
}

String authPayload() {
  String jwt = webServer.getCookie("esp.login");
  if (!jwt.length()) {
    webServer.send(401, "text/plain", "Unauthorized");
    jwt.clear();
    return "";
  }
  DynamicJsonDocument payloadDocument(MAX_JWT_SIZE);
  jwtHandler.parseJWT(jwt, payloadDocument);
  if (payloadDocument.isNull()) {
    jwt.clear();
    // webServer.setCookie("esp.login", "");
    webServer.send(403, "text/plain", "Forbidden");
    return "";
  }

  String payload;
  serializeJson(payloadDocument, payload);

  return payload;
}

void handleRoot() {
  Serial.println("GET /api");
  webServer.send(200, "application/json", "{\"message\":\"hello from esp8266!\", \"success\":true}");
}

void handleToggleLedState() {
  if (!ensureAuthenticated()) {
    return;
  }

  String ledState = dataFilesManager.load("led");
  bool newLedState = !ledState.toInt();

  dataFilesManager.save("led", String(newLedState));
  
  digitalWrite(LED_BUILTIN, newLedState ? LOW : HIGH);

  webServer.send(200, "application/json", "{\"state\": " + String(newLedState) + "}");
}

void handleGetLedState() {
  String ledState = dataFilesManager.load("led");

  webServer.send(200, "text/json", "{\"state\": " + ledState + "}");
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
  webServer.on("/api/session", HTTP_POST, []() {
    StaticJsonDocument<128> bodyObj;
    deserializeJson(bodyObj, webServer.body());

    if (bodyObj["username"] != SECRET_ADMIN_USER || bodyObj["password"] != SECRET_ADMIN_PASS) {
      webServer.send(401, "application/json", "{\"error\":\"Invalid username or password\"}");
      return;
    }

    DynamicJsonDocument jsonDocument(MAX_JWT_SIZE);
    JsonObject jwtObj = jsonDocument.to<JsonObject>();
    jwtObj["username"] = "admin";
    String json;
    serializeJson(jwtObj, json);
    String jwt = jwtHandler.buildJWT(jwtObj);
    webServer.setCookie("esp.login", jwt + "; Max-Age=31536000"); // 1 year expiration
    webServer.send(200, "application/json", "{\"user\":" + json + ",\"token\":\"" + jwt + "\"}");
    jwt.clear();
  });
  webServer.on("/api/profile", HTTP_GET, []() {
    String payload = authPayload();
    if (!payload) {
      payload.clear();
      return;
    }
    webServer.send(200, "application/json", payload);
    payload.clear();
  });
  webServer.on("/api/logout", HTTP_GET, []() {
    webServer.setCookie("esp.login", "; Max-Age=0");\
    webServer.send(200, "application/json", "{\"ok\":true\"}");
  });
  webServer.on("/api/led/toggle", HTTP_GET, handleToggleLedState);
  webServer.on("/api/led", HTTP_GET, handleGetLedState);
}

void loop() {
  otaManager.loop();
  webServer.handleClient();
}
