#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESP8266WebServer.h>
#include "LittleFS.h"

class WebServer {
public:
  WebServer(int port = 80);

  void begin();

  void handleClient();

  void on(const String &uri, HTTPMethod method, ESP8266WebServer::THandlerFunction handler);

  void send(int code, const String &content_type, const String &content);
  
  String body();

  void setCookie(const String name, const String value);

  String getCookie(const String name);

  String arg(const String &name);

private:
  ESP8266WebServer server;

  void handleNotFound();

  bool handleFileRead(String path);

  String getContentType(String filename);
};

#endif
