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

 ActionToServerBridge.cpp
 Created on: Sep 25, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#include "ActionToServerBridge.h"
#include <ActionsMgr.h>
#include <ActionBind.h>
#include <executables/HttpCommand.h>
#include "debug.h"

String ActionToServerBridge::actionsAsParams() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  for (int t = 0; t < 8; t++) {
    if ((actionsMgr.actions[t] != nullptr) && actionsMgr.actions[t]->cmd) {
      auto& obj = root.createNestedObject(String(t + 1));
      actionToJson(obj, actionsMgr.actions[t]);
    }
  }

  String result;
  root.printTo(result);
  return result;
}

void ActionToServerBridge::actionToJson(JsonObject& jsonNode,
    ActionBind* actionBind) {
  if (actionBind->cmd->getTypeId() == HTTP_COMMAND_TYPE_MARKER) {
    httpCommandToJson(jsonNode,
        reinterpret_cast<HttpCommand*>(actionBind->cmd));
  }
}

void ActionToServerBridge::httpCommandToJson(JsonObject& jsonNode,
    HttpCommand* cmd) {
  jsonNode["__U"] = cmd->url;
  if (cmd->usePost) {
    jsonNode["__P"] = 1;
  }
  if (cmd->keyLen > 0) {
    jsonNode["__S"] = 1;
    jsonNode["__K"] = toHexString(cmd->key, cmd->keyLen);
  }
  for (int t = 0; t < cmd->paramsCount(); t++) {
    std::pair<String, String>& param = cmd->param(t);
    jsonNode[param.first] = param.second;
  }
}

bool ActionToServerBridge::requestToActions(ESP8266WebServer& httpServer) {
  BindsMap binds;
  bool success = true;
  success = createBinds(httpServer, binds);

  for (int t = 0; success and (t < httpServer.args()); t++) {
    String name = httpServer.argName(t);
    LOG("Param:");
    LOG_LN(name);
    if (name.startsWith("sign-")) {
      int8_t index = static_cast<int8_t>(name.substring(5).toInt());
      String keyParamName{"key-"};
      keyParamName += index;
      success = httpServer.hasArg(keyParamName);
      LOG("SIGNATURE:");
      LOG(keyParamName);
      LOG("=");
      LOG(success);

      if (success) {
        success = setSignatureOn(binds, index, httpServer.arg(keyParamName));
      }

    } else if (name.startsWith("post-")) {
      int8_t index = static_cast<int8_t>(name.substring(5).toInt());
      LOG("POST Index:");
      LOG(index);

      success &= setPostOn(binds, index);

    } else if (name.startsWith("pn-")) {
      int8_t index = extractIndexFromParamArg(name);
      String keyParamName{"pv-"};
      keyParamName += name.substring(3);
      success = httpServer.hasArg(keyParamName);
      LOG("PARAM:");
      LOG(keyParamName);
      LOG("=");
      LOG(success);
      if (success) {
        success = addParamToCmd(binds, index, httpServer.arg(t),
            httpServer.arg(keyParamName));
      }
    }
  }

  if (not success) {
    releaseBinds(binds);
    return false;

  } else {
    actionsMgr.clear();
    for (auto it : binds) {
      actionsMgr.putAction(it.second);
    }
  }

  return success;
}

int8_t ActionToServerBridge::extractIndexFromParamArg(String& name) {
  int endIndex = name.indexOf('-', 3);
  String indexStr = name.substring(3, endIndex);
  return static_cast<int8_t>(indexStr.toInt());
}

bool ActionToServerBridge::addParamToCmd(BindsMap& binds, int8_t index,
    String param, String value) {
  auto iter = binds.find(index);
  if (iter == binds.end()) {
    //param can be sent for not created action, just ignore it or fix html
    return true;
  }
  if (((*iter).second->cmd == nullptr) or (param.length() == 0) or
      (value.length() == 0)) {
    return false;
  }
  HttpCommand* cmd = static_cast<HttpCommand*>((*iter).second->cmd);
  cmd->addData(param, value);
  LOG(", P[");
  LOG(param);
  LOG("]=");
  LOG_LN(value);
  return true;
}

bool ActionToServerBridge::setPostOn(BindsMap& binds, int8_t index) {
  auto iter = binds.find(index);
  if (iter == binds.end()) {
    //post can be sent for not created action, just ignore it or fix html
    return true;
  }
  if ((*iter).second->cmd == nullptr) {
    return false;
  }
  HttpCommand* cmd = static_cast<HttpCommand*>((*iter).second->cmd);
  cmd->usePost = true;
  LOG_LN("SET");
  return true;
}

bool ActionToServerBridge::setSignatureOn(BindsMap& binds, int8_t index,
    const String& key) {
  auto iter = binds.find(index);
  if (iter == binds.end()) {
    //key can be sent for not created action, just ignore it or fix html
    return true;
  }
  if (key.length() == 0) {
    return false;
  }
  HttpCommand* cmd = static_cast<HttpCommand*>((*iter).second->cmd);
  cmd->keyLen = fromHexString(key, &cmd->key);
  LOG(", KEY=");
  LOG(key);
  LOG(", keyLen=");
  LOG_LN(cmd->keyLen);

  return cmd->keyLen != 0;
}

void ActionToServerBridge::releaseBinds(BindsMap& binds) {
  for (auto it : binds) {
    delete it.second;
  }
}

bool ActionToServerBridge::createBinds(ESP8266WebServer& httpServer,
    BindsMap& binds) {
  for (int t = 0; t < httpServer.args(); t++) {
    String name = httpServer.argName(t);
    if (name.startsWith("url-")) {
      int8_t index = static_cast<int8_t>(name.substring(4).toInt());
      if (index == 0) {
        //this is invalid, indices start from 1.
        return false;
      }
      //if no url is given, this is not an error. Just don't do nothing with it
      if (httpServer.arg(t).length() == 0) {
        return true;
      }
      HttpCommand* cmd = new HttpCommand();
      cmd->url = httpServer.arg(t);
      //note params are 1 based, actions are 0 based
      binds[index] = new ActionBind(index - 1, cmd);
    }
  }
  return true;
}

String ActionToServerBridge::toHexString(uint8_t* data, int len) {
  String result;
  for (int t = 0; t < len; t++) {
    uint8_t b = (data[t] >> 4) & 0x0F;
    char c = (b < 0xA) ? '0' + b : 'A' + b - 0xA;
    result.concat(c);

    b = data[t] & 0x0F;
    c = (b < 0xA) ? '0' + b : 'A' + b - 0xA;
    result.concat(c);
  }

  return result;
}

bool handleSingleHex(const char& c, uint8_t& val) {
  if (c >= '0' && c <= '9') {
    val += c - '0';
  } else if (c >= 'A' && c <= 'F') {
    val += 10 + c - 'A';
  } else if (c >= 'a' && c <= 'f') {
    val += 10 + c - 'a';
  } else {
    return false;
  }

  return true;
}

int ActionToServerBridge::fromHexString(String hex, uint8_t** data) {
  if (hex.length() % 2 != 0) {
    *data = nullptr;
    return 0;
  }
  *data = new uint8_t[hex.length() / 2];
  int index = 0;
  for (auto it = hex.begin(); it != hex.end();) {
    uint8_t val = 0;

    bool success = handleSingleHex(*it, val);
    it++;
    val <<= 4;

    success &= handleSingleHex(*it, val);
    it++;

    if (not success) {
      delete[] *data;
      *data = nullptr;
      return 0;
    }

    (*data)[index] = val;
    index++;
  }
  return hex.length() / 2;
}
