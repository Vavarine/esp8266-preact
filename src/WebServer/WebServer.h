#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <ESPAsyncWebServer.h>

#include "LittleFS.h"
#include "utils/User.h"

enum Auth { REQUIRE_AUTH };

class WebServer {
public:
  WebServer(int port = 80);
  typedef std::function<void(AsyncWebServerRequest *request, User* user)> ArAuthenticatedRequestHandlerFunction;
  typedef std::function<void(AsyncWebServerRequest *request, String body)> ArRequestWithBodyHandlerFunction;
  typedef std::function<void(AsyncWebServerRequest *request, User* user, String body)> ArAuthenticatedRequestWithBodyHandlerFunction;

  void begin();

  void on(const String &uri, WebRequestMethod method, ArRequestHandlerFunction onRequest);
  void on(const String &uri, WebRequestMethod method, ArRequestWithBodyHandlerFunction onRequest);
  void on(const String &uri, WebRequestMethod method, Auth auth, ArRequestHandlerFunction onRequest);
  void on(const String &uri, WebRequestMethod method, Auth auth, ArRequestWithBodyHandlerFunction onRequest);
  void on(const String &uri, WebRequestMethod method, Auth auth, ArAuthenticatedRequestHandlerFunction onRequest);
  void on(const String &uri, WebRequestMethod method, Auth auth, ArAuthenticatedRequestWithBodyHandlerFunction onRequest);

  static void setCookie(AsyncWebServerResponse *response, const String name, const String value);

  static String getCookie(AsyncWebServerRequest* request, const String name);

private:
  AsyncWebServer server;

  void handleNotFound(AsyncWebServerRequest *request);

  bool handleFileRead(AsyncWebServerRequest *request);

  String getContentType(String filename);

  static bool ensureAuthenticated(AsyncWebServerRequest *request);
  static String getJWTPayload(AsyncWebServerRequest *request);
  static User* getUser(AsyncWebServerRequest *request);

  static void _bodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
