#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include "WebServer.h"
#include "ArduinoJsonJWT/ArduinoJsonJWT.h"
#include "utils/User.h"
#include "secrets.h"
#include "static_files.h"
#include <pgmspace.h>

WebServer::WebServer(int port) : server(port) {}

const String NO_AUTH_JWT_SECRET = "NO_AUTH";
const String NO_AUTH_JWT_PAYLOAD = "{\"username\":\"guest\",\"noAuth\":true}";
const String NO_AUTH_JWT_SIGNED = "eyJhbciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Imd1ZXN0Iiwibm9BdXRoIjp0cnVlfQ.cDADZah2PdnWgiQyp7lwc69m6BC9oIOlA-IuyH3sL4o";

#ifdef SECRET_JWT
ArduinoJsonJWT jwtHandler(SECRET_JWT);
#else
ArduinoJsonJWT jwtHandler(NO_AUTH_JWT_SECRET);
#endif

bool beginLittleFS() {
  #ifdef ESP32
    return LittleFS.begin(true);
  #elif defined(ESP8266)
    return LittleFS.begin();
  #endif
}

void WebServer::begin() {
  // Enable CORS
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  if (!beginLittleFS()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  server.on("/api/ping", HTTP_GET, [this](AsyncWebServerRequest* request) {
    Serial.println("GET /api/ping");
    request->send(200, "text/plain", "pong");
  });

  this->on("/api/session", HTTP_POST, [this](AsyncWebServerRequest* request, String body) {
    #ifdef SECRET_JWT
      JsonDocument bodyObj;
      deserializeJson(bodyObj, body);

      if (bodyObj["username"] != SECRET_ADMIN_USER || bodyObj["password"] != SECRET_ADMIN_PASS) {
        request->send(401, "application/json", "{\"error\":\"Invalid username or password\"}");
        return;
      }

      JsonDocument jsonDocument;
      JsonObject jwtObj = jsonDocument.to<JsonObject>();
      jwtObj["username"] = SECRET_ADMIN_USER;
      String json;
      serializeJson(jwtObj, json);
      String jwt = jwtHandler.buildJWT(jwtObj);
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"user\":" + json + ",\"token\":\"" + jwt + "\"}");
      this->setCookie(response, "esp.login", jwt + "; Max-Age=31536000; HttpOnly"); // 1 year expiration
      request->send(response);
      jwt.clear();
    #else
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"user\":" + NO_AUTH_JWT_PAYLOAD + ",\"token\":\"" + NO_AUTH_JWT_SIGNED + "\"}");
      this->setCookie(response, "esp.login", NO_AUTH_JWT_SIGNED + "; Max-Age=31536000; HttpOnly"); // 1 year expiration
      request->send(response);
    #endif
  });
  this->on("/api/profile", HTTP_GET, REQUIRE_AUTH, [this](AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", this->getJWTPayload(request));
    #ifndef SECRET_JWT
      this->setCookie(response, "esp.login", NO_AUTH_JWT_SIGNED + "; Max-Age=31536000; HttpOnly"); // 1 year expiration
    #endif
    request->send(response);
  });
  server.on("/api/logout", HTTP_GET, [this](AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"ok\":true\"}");
    #ifdef SECRET_JWT
      this->setCookie(response, "esp.login", "; Max-Age=0; HttpOnly");
    #endif
    request->send(response);
  });

  for (int i = 0; i < static_files::num_of_files; i++) {
    server.on(static_files::files[i].path, [this, i](AsyncWebServerRequest *request){
      String contentType = static_files::files[i].type;
      AsyncWebServerResponse *response = request->beginResponse_P(200, static_files::files[i].type, static_files::files[i].contents, static_files::files[i].size);
      response->addHeader("Content-Encoding", "gzip");
      if (contentType == "text/css" || contentType == "application/javascript") {
        response->addHeader("Cache-Control", "max-age=31536000");
      }
      request->send(response);
    });
  }

  // Serve static files from PROGMEM
  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", static_files::f_index_html_contents, static_files::f_index_html_size);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.onNotFound([this](AsyncWebServerRequest *request) {
    this->handleNotFound(request);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void WebServer::on(const String &uri, WebRequestMethod method, ArRequestHandlerFunction onRequest) {
  server.on(uri.c_str(), method, onRequest);
}

void WebServer::on(const String &uri, WebRequestMethod method, WebServer::ArRequestWithBodyHandlerFunction onRequest) {
  server.on(uri.c_str(), method, [onRequest](AsyncWebServerRequest *request) {
    String body = String((const char *)request->_tempObject);
    onRequest(request, body);
  }, NULL, _bodyHandler);
}

void WebServer::on(const String &uri, WebRequestMethod method, Auth auth, WebServer::ArRequestWithBodyHandlerFunction onRequest) {
  server.on(uri.c_str(), method, [this, auth, onRequest](AsyncWebServerRequest *request) {
    const char* body = (const char *)request->_tempObject;
    if (auth == REQUIRE_AUTH && !this->ensureAuthenticated(request)) {
      return;
    }
    onRequest(request, body);
  }, NULL, _bodyHandler);
}

void WebServer::on(const String &uri, WebRequestMethod method, Auth auth, ArRequestHandlerFunction onRequest) {
  server.on(uri.c_str(), method, [this, auth, onRequest](AsyncWebServerRequest *request) {
    if (auth == REQUIRE_AUTH && !this->ensureAuthenticated(request)) {
      return;
    }
    onRequest(request);
  });
}

void WebServer::on(const String &uri, WebRequestMethod method, Auth auth, WebServer::ArAuthenticatedRequestHandlerFunction onRequest) {
  if (auth == REQUIRE_AUTH) {
    server.on(uri.c_str(), method, [this, onRequest](AsyncWebServerRequest *request) {
      User* user = this->getUser(request);
      if (!user->username.length()) {
        return;
      }
      onRequest(request, user);
    });
  }
}
void WebServer::on(const String &uri, WebRequestMethod method, Auth auth, WebServer::ArAuthenticatedRequestWithBodyHandlerFunction onRequest) {
  if (auth == REQUIRE_AUTH) {
    this->on(uri.c_str(), method, [this, onRequest](AsyncWebServerRequest *request, String body) {
      User* user = this->getUser(request);
      if (!user->username.length()) {
        return;
      }
      onRequest(request, user, body);
    });
  }
}

void WebServer::setCookie(AsyncWebServerResponse *response, const String name, const String value) {
  response->addHeader("Set-Cookie", name + "=" + value + ";");
}

String WebServer::getCookie(AsyncWebServerRequest* request, const String name) {
  if (request->hasHeader("Cookie")) {
    AsyncWebHeader* h = request->getHeader("Cookie");
    String cookie = h->value();
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

void WebServer::handleNotFound(AsyncWebServerRequest *request) {
  if (request->method() != HTTP_GET) {
    request->send(404, "text/plain", "Not found");
    return;
  }

  if (!handleFileRead(request)) {
    // return index.html if it doesn't exist
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", static_files::f_index_html_contents, static_files::f_index_html_size);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  }
}

bool WebServer::handleFileRead(AsyncWebServerRequest *request) {
  String path = request->url();
  if (path.endsWith("/")) {
    path += "index.html";
  }

  
  if (!LittleFS.exists(path)) {
    return false;
  }

  String contentType = getContentType(path);

  AsyncWebServerResponse *response = request->beginResponse(LittleFS, path, contentType);

  if (contentType == "text/css" || contentType == "application/javascript") {
    response->addHeader("Cache-Control", "max-age=31536000");
  }

  request->send(response);
  return true;
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

void WebServer::_bodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (!request->_tempObject)
  {
      request->_tempObject = new char[total + 1];
  }
  char *tempObject = (char *)request->_tempObject;
  for (unsigned int i = 0; i < len; i++)
  {
      tempObject[index + i] = (char)data[i];
  }
  tempObject[total] = '\0';
  Serial.println("Parsing request body: " + String(tempObject));
}

bool WebServer::ensureAuthenticated(AsyncWebServerRequest *request) {
  #ifdef SECRET_JWT
    String jwt = getCookie(request, "esp.login");
    if (!jwt.length()) {
      request->send(401, "text/plain", "Unauthorized");
      // jwt.clear();
      return false;
    }
    JsonDocument payloadDocument;
    jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.isNull()) {
      request->send(403, "text/plain", "Forbidden");
      // jwt.clear();
      return false;
    }
    // jwt.clear();
    return true;
  #else
    return true;
  #endif
}

String WebServer::getJWTPayload(AsyncWebServerRequest *request) {
  #ifdef SECRET_JWT
    String jwt = getCookie(request, "esp.login");
    if (!jwt.length()) {
      request->send(401, "text/plain", "Unauthorized");
      jwt.clear();
      return emptyString;
    }
    JsonDocument payloadDocument;
    jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.isNull()) {
      jwt.clear();
      // this->setCookie("esp.login", "");
      request->send(403, "text/plain", "Forbidden");
      return emptyString;
    }

    String payload;
    serializeJson(payloadDocument, payload);

    return payload;
  #else
    return NO_AUTH_JWT_PAYLOAD;
  #endif
}

User* WebServer::getUser(AsyncWebServerRequest *request) {
  #ifdef SECRET_JWT
    String jwt = getCookie(request, "esp.login");
    if (!jwt.length()) {
      request->send(401, "text/plain", "Unauthorized");
      jwt.clear();
      return new User(emptyString);
    }
    JsonDocument payloadDocument;
    jwtHandler.parseJWT(jwt, payloadDocument);
    if (payloadDocument.isNull()) {
      jwt.clear();
      // this->setCookie("esp.login", "");
      request->send(403, "text/plain", "Forbidden");
      return new User(emptyString);
    }
  #else
    JsonDocument payloadDocument;
    deserializeJson(payloadDocument, NO_AUTH_JWT_PAYLOAD);
  #endif

  return new User(payloadDocument["username"].as<String>());
}
