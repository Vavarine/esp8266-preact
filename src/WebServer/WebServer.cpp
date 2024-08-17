#include <ESP8266WebServer.h>
#include "LittleFS.h"
#include "WebServer.h"

WebServer::WebServer(int port) : server(port) {}

void WebServer::begin() {
  server.collectHeaders("Cookie");
  server.begin();
  server.enableCORS(true);

  if(!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  server.onNotFound([this]() {
    this->handleNotFound();
  });

  Serial.println("HTTP server started");
}

void WebServer::handleClient() {
  server.handleClient();
}

void WebServer::on(const String &uri, HTTPMethod method, ESP8266WebServer::THandlerFunction handler) {
  server.on(uri, method, [handler]() {
    handler();
  });
}

void WebServer::send(int code, const String &content_type, const String &content) {
  server.send(code, content_type, content);
}

String WebServer::body() {
  return server.arg("plain");
}

void WebServer::setCookie(const String name, const String value) {
  server.sendHeader("Set-Cookie", name + "=" + value + ";");
}

String WebServer::getCookie(const String name) {
  if (server.hasHeader("Cookie")) {
    
    String cookie = server.header("Cookie");
    int keyIndex = cookie.indexOf(name + "=");
    if (keyIndex == -1) {
      return ""; // Key not found
    }
    int valueStart = keyIndex + name.length() + 1;
    int valueEnd = cookie.indexOf(';', valueStart);
    if (valueEnd == -1) {
      valueEnd = cookie.length();
    }
    return cookie.substring(valueStart, valueEnd);
  }
  return "";
}

String WebServer::arg(const String &name) {
  return server.arg(name);
}

void WebServer::handleNotFound() {
  if (!handleFileRead(server.uri())) {
    // return index.html if it doesn't exist
    handleFileRead("/index.html");
  }
}

bool WebServer::handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "index.html";
  }

  String contentType = getContentType(path);

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);

    file.close();
    return true;
  }

  return false;
}

String WebServer::getContentType(String filename) {
  // if (server.hasArg("download")) return "application/octet-stream";
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".svg"))
    return "image/svg+xml";

  return "text/plain";
}
