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

 HttpCommand.cpp
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include "HttpCommand.h"
#include <ArduinoJson.h>
#include <sha256.h>
#include <ESP8266TrueRandom.h>
#include "FileHelper.h"
#include <ESP8266WiFi.h>
#include <network/NetworkCtrl.h>
#include "debug.h"
/*
  uint8_t pass[] = {1,2,3,4};
  HttpCommand cmd("http://ptsv2.com/t/5kpry-1533759192/post", pass, 4, false);
  cmd.addData("dupa", "blada");
  int r = cmd.execute();
 */

HttpCommand::HttpCommand(String url, uint8_t* key, uint8_t keyLen, bool usePost)
: url(url), key(key), keyLen(keyLen), usePost(usePost) {
}

HttpCommand::HttpCommand()
: key(nullptr), keyLen(0), usePost(false) {
}

HttpCommand::HttpCommand(File& file) {
  key = nullptr;
  readString(file, url);
  readPrimitive(file, keyLen);
  if (keyLen > 0) {
    key = (uint8_t*)malloc(keyLen);
    file.read(key, keyLen);
  }
  readPrimitive(file, usePost);
  uint8_t tmp = 0;
  readPrimitive(file, tmp);
  for(int t = 0; t < tmp; t++) {
    String param;
    String value;
    readString(file, param);
    readString(file, value);
    data.push_back(std::make_pair(param, value));
  }
}

HttpCommand::~HttpCommand() {
  if (key != nullptr) {
    delete key;
  }
}

void HttpCommand::addData(String param, String value) {
  data.push_back(std::make_pair(param, value));
}

void HttpCommand::calcHMac(String& nonce, String& hmac) {
  Sha256.initHmac(key, keyLen);
  Sha256.print(nonce);
  for(auto it = data.begin(); it != data.end(); it++) {
    Sha256.print(it->first);
    Sha256.print(it->second);
  }
  Sha256.print(nonce);

  hmac.reserve(64);
  uint8_t* hash = Sha256.resultHmac();
  for (int t = 0; t < 32; t++) {
    hmac.concat( (char)(65 + (hash[t]>>4)));
    hmac.concat( (char)(65 + (hash[t]&0xF)));
  }
}

void HttpCommand::dropResponse() {
  response = "";
}

bool HttpCommand::execute() {

  if (networkCtrl.waitForAPConnection() == false) {
    return false;
  }

  HTTPClient http;

  int httpCode = usePost ? doPost(http) : doGet(http);
  response = http.getString();

  http.end();

  return httpCode == 200;
}

int HttpCommand::doPost(HTTPClient& http) {
  String payload;
  buildQuery(payload);
  String hMac;
  //TODO: well, nonce is bullshit in this form. But how to go with no RTC?
  String nonce(ESP8266TrueRandom.random() % 65535);
  calcHMac(nonce, hMac);

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("HMac", hMac);
  http.addHeader("nonce", nonce);
  return http.POST(payload);
}

void HttpCommand::urlEncoded(String& in, String& out) {
  static const char* hex = "0123456789abcdef";
  out.reserve(in.length());  //be optimistic :D

  for(auto it = in.begin(); it != in.end(); it++){
    char c = *it;

    if( ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') ) {
      out.concat(c);
    } else {
      out.concat('%');
      out.concat(hex[c >> 4]);
      out.concat(hex[c & 15]);
    }
  }
}
void HttpCommand::buildQuery(String& query) {
  if (data.size() > 0) {
    int estSize = 0;
    for(auto it = data.begin(); it != data.end(); it++) {
      estSize += it->first.length() + it->second.length() + 2;  //+2 -> &/? and = sign
    }
    query.reserve(estSize);

    for(auto it = data.begin(); it != data.end(); it++) {
      query += '&';
      urlEncoded(it->first, query);
      query += '=';
      urlEncoded(it->second, query);
    }
  }
}

int HttpCommand::doGet(HTTPClient& http) {
  String query;
  buildQuery(query);

  String hMac;
  //TODO: well, nonce is bullshit in this form. But how to go with no RTC?
  String nonce(ESP8266TrueRandom.random() % 65535);
  calcHMac(nonce, hMac);

  if (url.endsWith("??")) {
    //nasty hack, prof of my lazines
    query += "&HMac=" + hMac +"&nonce="+nonce;
    http.begin(url.substring(0, url.length() - 2) + "/" + query);
  } else {
    http.begin(url + "?" + query);
    http.addHeader("HMac", hMac);
    http.addHeader("nonce", nonce);
  }
  return http.GET();
}

String& HttpCommand::getResponse() {
  return response;
}

void HttpCommand::serialize(File& file) {
  writeString(file, url);
  writePrimitive(file, keyLen);
  if (keyLen > 0) {
    file.write(key, keyLen);
  }
  writePrimitive(file, usePost);
  uint8_t tmp = data.size();
  writePrimitive(file, tmp);
  for(auto it = data.begin(); it != data.end(); it++) {
    writeString(file, it->first);
    writeString(file, it->second);
  }
}
