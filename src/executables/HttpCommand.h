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

 HttpCommand.h
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef HttpCommand_hpp
#define HttpCommand_hpp

#include <Arduino.h>
#include <vector>
#include <ESP8266HTTPClient.h>
#include "Executable.h"

#define HTTP_COMMAND_TYPE_MARKER 'H'

class HttpCommand : public Executable {
  public:
    HttpCommand(String url, uint8_t* key, uint8_t keyLen, bool usePost);
    HttpCommand(File& file);
    void addData(String param, String value);
    bool execute() override;
    virtual void serialize(File& file) override;
    String& getResponse();
    void dropResponse();
  private:
    String url;
    uint8_t* key;
    uint8_t keyLen;
    bool usePost;
    String response;
    std::vector<std::pair<String, String>> data;

    void calcHMac(String& data, String& nonce, String& hmac);

    void buildPayload(String& data);
    int doPost(HTTPClient& http);

    void buildQuery(String& query);
    void urlEncoded(String& in, String& out);
    int doGet(HTTPClient& http);
};

#endif /* HttpCommand_hpp */
