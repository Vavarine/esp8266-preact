#include <ESP8266WebServer.h>
#include "LittleFS.h"
#include "WebServer.h"
#include "ArduinoJsonJWT/ArduinoJsonJWT.h"
#include "secrets.h"

WebServer::WebServer(int port) : server(port) {}

const String NO_AUTH_JWT_SECRET = "NO_AUTH";
const String NO_AUTH_JWT_PAYLOAD = "{\"username\":\"guest\",\"noAuth\":true}";
const String NO_AUTH_JWT_SIGNED = "eyJhbciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Imd1ZXN0Iiwibm9BdXRoIjp0cnVlfQ.cDADZah2PdnWgiQyp7lwc69m6BC9oIOlA-IuyH3sL4o";

#ifdef SECRET_JWT
ArduinoJsonJWT jwtHandler(SECRET_JWT);
#else
ArduinoJsonJWT jwtHandler(NO_AUTH_JWT_SECRET);
#endif

void WebServer::begin() {
  server.collectHeaders("Cookie");
  server.begin();
  server.enableCORS(true);

  if(!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  server.on("/api/session", HTTP_POST, [this]() {
    #ifdef SECRET_JWT
      JsonDocument bodyObj;
      deserializeJson(bodyObj, this->body());

      if (bodyObj["username"] != SECRET_ADMIN_USER || bodyObj["password"] != SECRET_ADMIN_PASS) {
        this->send(401, "application/json", "{\"error\":\"Invalid username or password\"}");
        return;
      }

      JsonDocument jsonDocument;
      JsonObject jwtObj = jsonDocument.to<JsonObject>();
      jwtObj["username"] = "admin";
      String json;
      serializeJson(jwtObj, json);
      String jwt = jwtHandler.buildJWT(jwtObj);
      this->setCookie("esp.login", jwt + "; Max-Age=31536000"); // 1 year expiration
      this->send(200, "application/json", "{\"user\":" + json + ",\"token\":\"" + jwt + "\"}");
      jwt.clear();
    #else
      this->setCookie("esp.login", NO_AUTH_JWT_SIGNED + "; Max-Age=31536000"); // 1 year expiration
      this->send(200, "application/json", NO_AUTH_JWT_PAYLOAD);
    #endif
  });
  this->on("/api/profile", HTTP_GET, REQUIRE_AUTH, [this](String payload) {
    #ifndef SECRET_JWT
      this->setCookie("esp.login", NO_AUTH_JWT_SIGNED + "; Max-Age=31536000"); // 1 year expiration
    #endif

    this->send(200, "application/json", payload);
  });
  server.on("/api/logout", HTTP_GET, [this]() {
    #ifdef SECRET_JWT
      this->setCookie("esp.login", "; Max-Age=0");
    #endif
    this->send(200, "application/json", "{\"ok\":true\"}");
  });

  server.onNotFound([this]() {
    this->handleNotFound();
  });

  Serial.println("HTTP server started");
}

void WebServer::handleClient() {
  server.handleClient();
}

void WebServer::on(const String &uri, HTTPMethod method, ESP8266WebServer::THandlerFunction handler) {
  server.on(uri, method, handler);
}

void WebServer::on(const String &uri, HTTPMethod method, Auth auth, ESP8266WebServer::THandlerFunction handler) {
  if (auth == REQUIRE_AUTH) {
    server.on(uri, method, [this, handler]() {
      if (!this->ensureAuthenticated()) {
        return;
      }
      handler();
    });
  } else {
    server.on(uri, method, handler);
  }
}

void WebServer::on(const String &uri, HTTPMethod method, Auth auth, WebServer::TAuthenticatedHandlerFunction handler) {
  if (auth == REQUIRE_AUTH) {
    server.on(uri, method, [this, handler]() {
      String payload = this->authPayload();
      if (!payload) {
        payload.clear();
        return;
      }
      handler(payload);
    });
  }
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

  if (contentType == "text/css" || contentType == "application/javascript") {
    server.sendHeader("Cache-Control", "max-age=31536000");
  }

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

bool WebServer::ensureAuthenticated() {
  #ifdef SECRET_JWT
    String jwt = this->getCookie("esp.login");
    if (!jwt.length()) {
      this->send(401, "text/plain", "Unauthorized");
      // jwt.clear();
      return false;
    }
    JsonDocument payloadDocument;
    jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.isNull()) {
      this->send(403, "text/plain", "Forbidden");
      // jwt.clear();
      return false;
    }
    // jwt.clear();
    return true;
  #else
    return true;
  #endif
}

String WebServer::authPayload() {
  #ifdef SECRET_JWT
    String jwt = this->getCookie("esp.login");
    if (!jwt.length()) {
      this->send(401, "text/plain", "Unauthorized");
      jwt.clear();
      return "";
    }
    JsonDocument payloadDocument;
    jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.isNull()) {
      jwt.clear();
      // this->setCookie("esp.login", "");
      this->send(403, "text/plain", "Forbidden");
      return "";
    }

    String payload;
    serializeJson(payloadDocument, payload);

    return payload;
  #else
    return NO_AUTH_JWT_PAYLOAD;
  #endif
}
