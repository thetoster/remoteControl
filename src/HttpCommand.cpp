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
#include <HttpCommand.h>
#include <ArduinoJson.h>
#include <sha256.h>
#include <ESP8266TrueRandom.h>

HttpCommand::HttpCommand(String url, uint8_t* key, uint8_t keyLen, bool usePost)
: url(url), key(key), keyLen(keyLen), usePost(usePost) {
}

void HttpCommand::addData(String param, String value) {
  data.push_back(std::make_pair(param, value));
}

void HttpCommand::buildPayload(String& payload) {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  for(auto it = data.begin(); it != data.end(); it++) {
    root[it->first] = it->second;
  }
  root.printTo(payload);
}

void HttpCommand::calcHMac(String& data, String& nonce, String& hmac) {
  Sha256.initHmac(key, keyLen);
  Sha256.print(nonce);
  Sha256.print(data);
  Sha256.print(nonce);

  hmac.reserve(64);
  uint8_t* hash = Sha256.resultHmac();
  for (int t = 0; t < 32; t++) {
    hmac.concat( (char)(65 + (hash[t]>>4)));
    hmac.concat( (char)(65 + (hash[t]&0xF)));
  }
}

bool HttpCommand::execute() {
  HTTPClient http;

  int httpCode = usePost ? doPost(http) : doGet(http);
  response = http.getString();

  http.end();

  return httpCode == 200;
}

int HttpCommand::doPost(HTTPClient& http) {
  String payload;
  buildPayload(payload);
  String hMac;
  String nonce(ESP8266TrueRandom.random() % 65535);
  calcHMac(payload, nonce, hMac);

  http.begin(url);
  http.addHeader("Content-Type", "text/plain");
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
    char separator = '?';

    for(auto it = data.begin(); it != data.end(); it++) {
      query += separator;
      urlEncoded(it->first, query);
      query += '=';
      urlEncoded(it->second, query);

      separator = '&';
    }
  }
}

int HttpCommand::doGet(HTTPClient& http) {
  String query;
  buildQuery(query);

  String hMac;
  String nonce(ESP8266TrueRandom.random() % 65535);
  calcHMac(query, nonce, hMac);

  http.begin(url + query);
  http.addHeader("HMac", hMac);
  http.addHeader("nonce", nonce);
  return http.GET();
}

String& HttpCommand::getResponse() {
  return response;
}
