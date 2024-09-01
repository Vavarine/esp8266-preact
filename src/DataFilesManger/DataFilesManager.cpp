#include "DataFilesManager.h"
#include "LittleFS.h"

DataFilesManager::DataFilesManager(const String &baseDir) : baseDirectory(baseDir) {}

void DataFilesManager::begin() {
  if(!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
}

void DataFilesManager::save(const String &filename, const String &content) {
  const String path = baseDirectory + "/" + filename;
  #ifdef ESP32
    File file = LittleFS.open(path, "w", true);
  #elif defined(ESP8266)
    File file = LittleFS.open(path, "w");
  #endif
  file.print(content);
  file.close();
}

String DataFilesManager::load(const String &filename) {
  const String path = baseDirectory + "/" + filename;

  if(!LittleFS.exists(path)) {
    return "";
  }

  File file = LittleFS.open(path, "r");
  String content = file.readString();
  file.close();

  return content;
}

void DataFilesManager::remove(const String &filename) {
  const String path = baseDirectory + "/" + filename ;
  LittleFS.remove(path);
}

void DataFilesManager::list() {
  #ifdef ESP32
    File root = LittleFS.open(baseDirectory);
    File file;
    while (file = root.openNextFile("r")) {
      Serial.print(file.name());
      Serial.print(" - ");
      Serial.println(file.size());
      file.close();
    }
  #elif defined(ESP8266)
    Dir dir = LittleFS.openDir(baseDirectory);

    while(dir.next()) {
      Serial.print(dir.fileName());
      Serial.print(" - ");
      File file = dir.openFile("r");
      Serial.println(file.size());
      file.close();
    }
  #endif
}