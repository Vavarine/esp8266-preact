#include "OTAManager.h"
#include <ArduinoOTA.h>

OTAManager::OTAManager() {}

void::OTAManager::begin() {
  #ifdef SECRET_OTA_PASSWORD
    ArduinoOTA.setPassword((const char *)SECRET_OTA_PASSWORD);
  #endif

  ArduinoOTA.setPort(8266);

  ArduinoOTA.onStart([]() {
    Serial.println("Received OTA request");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA finished");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void::OTAManager::loop() {
  ArduinoOTA.handle();
}