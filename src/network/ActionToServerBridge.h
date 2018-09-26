/*
 BSD 3-Clause License

 Copyright (c) 2017, The Tosters
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ActionToServerBridge.h
 Created on: Sep 25, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#ifndef SRC_NETWORK_ACTIONTOSERVERBRIDGE_H_
#define SRC_NETWORK_ACTIONTOSERVERBRIDGE_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <unordered_map>

class ActionBind;
class HttpCommand;

class ActionToServerBridge {
public:
  String actionsAsParams();
  bool requestToActions(ESP8266WebServer& httpServer);

private:
  using BindsMap = std::unordered_map<int8_t, ActionBind*>;

  String toHexString(uint8_t* data, int len);
  int fromHexString(String hex, uint8_t** data);
  void actionToJson(JsonObject& jsonNode, ActionBind* actionBind);
  void httpCommandToJson(JsonObject& jsonNode, HttpCommand* cmd);
  bool createBinds(ESP8266WebServer& httpServer, BindsMap& binds);
  void releaseBinds(BindsMap& binds);
  bool setSignatureOn(BindsMap& binds, int8_t index, const String& key);
  bool setPostOn(BindsMap& binds, int8_t index);
  bool addParamToCmd(BindsMap& binds, int8_t index, String param, String value);
  int8_t extractIndexFromParamArg(String& name, int start);
  bool setLCDLines(BindsMap& binds, int8_t index, String line1, String line2);
};

#endif /* SRC_NETWORK_ACTIONTOSERVERBRIDGE_H_ */
