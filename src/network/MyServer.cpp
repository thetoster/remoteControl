/*
 MIT License

Copyright (c) 2018 Bartłomiej Żarnowski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 WifiServer.cpp
 Created on: Dec 29, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <ESP8266TrueRandom.h>
#include "MyServer.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Updater.h"
#include "../Prefs.h"
#include <ConfigMgr.h>
#include "../debug.h"

static const char rootHtml[] PROGMEM =
  #include "www/index_comp.html"
;

static const char netSetupHtml[] PROGMEM =
    #include "www/netSetup_comp.html"
;

ESP8266WebServer httpServer(80);
MyServer myServer;

static const char* www_username = "Lampster";
static const char* www_realm = "Remote Control";

namespace {

bool checkAuth() {
  const char* user = prefs.hasPrefs() ? prefs.storage.username : www_username;

  if (httpServer.authenticate(user, prefs.storage.password) == false) {
    httpServer.requestAuthentication(DIGEST_AUTH, www_realm);
    return false;
  };
  return true;
}

void handleNotFound(){
  if (checkAuth() == false) {
    return;
  }
  httpServer.send(404, "text/plain", "404: Not found");
}

void handleFactoryConfig() {
  if (checkAuth() == false) {
    return;
  }
  //redirect to confirMgr?
  prefs.defaultValues();
  prefs.save();
  httpServer.send(200, "text/plain", "200: OK");
  myServer.switchToConfigMode();
}

void handleVersion() {
  if (checkAuth() == false) {
    return;
  }
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["version"] = versionString;
  String response;
  root.printTo(response);
  httpServer.send(200, "application/json", response);
}

void handleGetNetConfig() {
  if (checkAuth() == false) {
    return;
  }
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  //Network
  root["ssid"] = prefs.storage.ssid;
  root["inNetworkName"] = prefs.storage.inNetworkName;
  root["username"] = prefs.storage.username;

  String response;
  root.printTo(response);
  httpServer.send(200, "application/json", response);
}

void handleNetSetup() {
  if (checkAuth() == false) {
    return;
  }
  String html = FPSTR(netSetupHtml);

  //network
  html.replace("${ssid}", prefs.storage.ssid);
  html.replace("${inNetworkName}", prefs.storage.inNetworkName);
  html.replace("${username}", prefs.storage.username);

  httpServer.send(200, "text/html", html);
}

String getStringArg(String argName, int maxLen, bool* isError) {
  String result = "";
  *isError = false;
  if (httpServer.hasArg(argName)) {
    result = httpServer.arg(argName);
    if (result.length() >= (unsigned int)maxLen) {
      String resp = "406: Not Acceptable, '" + argName + "' to long.";
      httpServer.send(406, "text/plain", resp);
      *isError = true;
    }
  }
  return result;
}

int getIntArg(String argName, int maxValue, bool* isError) {
  int result = -1;
  *isError = false;
  if (httpServer.hasArg(argName)) {
    result = httpServer.arg(argName).toInt();
    if (result >= maxValue) {
      String resp = "406: Not Acceptable, '" + argName + "' to big.";
      httpServer.send(406, "text/plain", resp);
      *isError = true;
    }
  }
  return result;
}

bool emplaceChars(char* ptr, String argName, int maxLen) {
  bool fail;
  String tmp = getStringArg(argName, maxLen, &fail);
  if (not fail) {
    strncpy(ptr, tmp.c_str(), maxLen);
  }
  return fail;
}

bool handleNetworkConfig(SavedPrefs& p) {
  bool fail = false;

  fail |= emplaceChars(p.ssid, "ssid", sizeof(p.ssid));
  fail |= emplaceChars(p.password, "password", sizeof(p.password));
  fail |= emplaceChars(p.inNetworkName, "inNetworkName", sizeof(p.inNetworkName));
  fail |= emplaceChars(p.username, "username", sizeof(p.username));
  fail |= emplaceChars(p.wifiPassword, "wifiPassword", sizeof(p.wifiPassword));

  return fail;
}

void applyNetConfig(SavedPrefs& p, bool& changed) {

  if ((strncmp(prefs.storage.inNetworkName, p.inNetworkName, sizeof(p.inNetworkName)) != 0)
      && (strnlen(p.inNetworkName, sizeof(p.inNetworkName)) > 0)) {
    strncpy(prefs.storage.inNetworkName, p.inNetworkName, sizeof(prefs.storage.inNetworkName));
    changed = true;
  }
  if ((strncmp(prefs.storage.password, p.password, sizeof(prefs.storage.password)) != 0)
     && (strnlen(p.password, sizeof(p.password)) > 0)) {
    strncpy(prefs.storage.password, p.password, sizeof(prefs.storage.password));
    changed = true;
  }
  if ((strncmp(prefs.storage.ssid, p.ssid, sizeof(prefs.storage.ssid)) != 0)
      && (strnlen(p.ssid, sizeof(p.ssid)) > 0)) {
    strncpy(prefs.storage.ssid, p.ssid, sizeof(prefs.storage.ssid));
    changed = true;
  }
  if ((strncmp(prefs.storage.username, p.username, sizeof(prefs.storage.username)) != 0)
       && (strnlen(p.username, sizeof(p.username)) > 0)) {
      strncpy(prefs.storage.username, p.username, sizeof(prefs.storage.username));
      changed = true;
  }
  if ((strncmp(prefs.storage.wifiPassword, p.wifiPassword, sizeof(prefs.storage.wifiPassword)) != 0)
       && (strnlen(p.wifiPassword, sizeof(p.wifiPassword)) > 0)) {
      strncpy(prefs.storage.wifiPassword, p.wifiPassword, sizeof(prefs.storage.wifiPassword));
      changed = true;
  }
}

template<typename T>
void applyIfChanged(T& from, T& to, bool& changed) {
  if (from != to) {
    to = from;
    changed = true;
  }
}

bool applyPrefsChange(SavedPrefs& p, bool& restartNetwork) {
  applyNetConfig(p, restartNetwork);

  bool changed = false;

  return changed | restartNetwork;
}

void handleSetNetConfig() {
  if (checkAuth() == false) {
    return;
  }

  SavedPrefs p = {0};

  bool fail = handleNetworkConfig(p);

  if (fail) {
    return;
  }

  //now apply new values
  bool restartNetwork = false;
  bool changed = applyPrefsChange(p, restartNetwork);

  String result = "200: OK";
  if (changed) {
    prefs.save();
    result += ", Config Saved";
  }

  delay(200);
  if (restartNetwork) {
    result += ", Network restarted";
  }
  httpServer.send(200, "text/plain", result);
  if (restartNetwork) {
  	configMgr.end();
		if (prefs.hasPrefs()) {
			myServer.connectToAccessPoint();
		}
  }
}

void handleRoot() {
  if (checkAuth() == false) {
    return;
  }
  //put config inside
  String html = FPSTR(rootHtml);
  String labels, temps, hums;
  httpServer.send(200, "text/html", html);
}

void handleUpdate() {
  if (checkAuth() == false) {
    return;
  }
  bool fail = false;
  String url = getStringArg("url", 1024, &fail);
  if (fail == false && url.length() > 0) {
      httpServer.send(200, "text/plain", "200: OK");
      updater.execute(url);

  } else {
    httpServer.send(400, "text/plain", "400: BAD REQUEST");
  }
}

}//namespace

void MyServer::switchToConfigMode() {
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(false);
  WiFi.enableAP(false);
  WiFi.enableSTA(false);
  delay(500);
  memset(prefs.storage.ssid, 0, sizeof(prefs.storage.ssid));
  generateRandomPassword();
  needsConfig = true;
  enableSoftAP();
}

void MyServer::connectToAccessPoint() {
  WiFi.softAPdisconnect(false);
  WiFi.enableAP(false);
  WiFi.begin(prefs.storage.ssid, prefs.storage.wifiPassword);
  WiFi.setAutoReconnect(true);
  WiFi.setAutoConnect(true);
//Dangerous, beware!
//  LOG("Connect to access point:'");
//  LOG(prefs.storage.ssid);
//  LOG("', password:'");
//  LOG(prefs.storage.wifiPassword);
//  LOG_LN("'");
}

void MyServer::generateRandomPassword() {
  memset(prefs.storage.password, 0, sizeof(prefs.storage.password));
  for(int t = 0; t < 8; t++) {
    int r = ESP8266TrueRandom.random(10);
    if (r < 3) {
      prefs.storage.password[t] = ESP8266TrueRandom.random('Z'-'A') + 'A';

    } else if (r < 6) {
      prefs.storage.password[t] = ESP8266TrueRandom.random('9'-'0') + '0';

    } else {
      prefs.storage.password[t] = ESP8266TrueRandom.random('z'-'a') + 'a';
    }
  }
//Nice for debug:
strcpy(prefs.storage.password, "TestTest");
}

String MyServer::getServerIp() {
  return needsConfig ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
}

bool MyServer::isServerConfigured() {
  return needsConfig == false;
}

String MyServer::getPassword() {
  return prefs.storage.password[0] == 0 ? "<-->" : String(prefs.storage.password);
}

void MyServer::enableSoftAP() {
  WiFi.softAP(prefs.storage.inNetworkName, prefs.storage.password);
}

void MyServer::restart() {
  httpServer.stop();
  WiFi.softAPdisconnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.setAutoConnect(false);
  WiFi.disconnect(false);
  WiFi.enableAP(false);
  WiFi.enableSTA(false);

  needsConfig = not prefs.hasPrefs();
  if (needsConfig) {
    generateRandomPassword();
    enableSoftAP();

  } else {
    connectToAccessPoint();
  }
  httpServer.on("/", handleRoot);
  httpServer.on("/network", HTTP_GET, handleGetNetConfig); //get current net conf as Json
  httpServer.on("/network", HTTP_POST, handleSetNetConfig); //set new net conf and restart
  httpServer.on("/setup", HTTP_GET, handleNetSetup);  //get page for net configuration
  httpServer.on("/factoryReset", handleFactoryConfig);
  httpServer.on("/update", HTTP_POST, handleUpdate);
  httpServer.on("/version", HTTP_GET, handleVersion);
  httpServer.on("/params", HTTP_GET, [this](){
  	httpServer.send(200, "application/json", atsBridge->actionsAsParams());
  }); //get buttons config as JSON

//  httpServer.on("/params", HTTP_POST, handleSetButtonActions); //set buttons config
  httpServer.onNotFound(handleNotFound);

  httpServer.begin();
  MDNS.notifyAPChange();
  MDNS.begin(prefs.storage.inNetworkName);
  MDNS.addService("http", "tcp", 80);
  LOG("Server MDNS:");
  LOG_LN(prefs.storage.inNetworkName);
}

void MyServer::update() {
  if (enabled) {
    MDNS.update();
    httpServer.handleClient();
  }
}

void MyServer::begin() {
  enabled = true;
  LOG_LN("Server -> Start");
  restart();
}

void MyServer::end() {
	LOG_LN("Server -> close");
  httpServer.stop();
  WiFi.setAutoConnect(true);
  enabled = false;
}
