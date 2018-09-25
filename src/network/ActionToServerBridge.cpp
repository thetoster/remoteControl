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

String ActionToServerBridge::actionsAsParams() {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  for(int t = 0; t < 8; t ++) {
  	if ((actionsMgr.actions[t] != nullptr) && actionsMgr.actions[t]->cmd) {
  		auto& obj = root.createNestedObject(String(t + 1));
  		actionToJson(obj, actionsMgr.actions[t]);
  	}
  }

  String result;
  root.printTo(result);
  return result;
}

void ActionToServerBridge::actionToJson(JsonObject& jsonNode, ActionBind* actionBind) {
	if (actionBind->cmd->getTypeId() == HTTP_COMMAND_TYPE_MARKER) {
		httpCommandToJson(jsonNode, reinterpret_cast<HttpCommand*>(actionBind->cmd));
	}
}

void ActionToServerBridge::httpCommandToJson(JsonObject& jsonNode, HttpCommand* cmd) {
	jsonNode["__U"] = cmd->url;
	if (cmd->usePost) {
		jsonNode["__P"] = 1;
	}
	if (cmd->keyLen > 0) {
		jsonNode["__S"] = 1;
		jsonNode["__K"] = toHexString(cmd->key, cmd->keyLen);
	}
	for(int t = 0; t < cmd->paramsCount(); t++) {
		std::pair<String, String>& param = cmd->param(t);
		jsonNode[param.first] = param.second;
	}
}

void ActionToServerBridge::paramsToActions(const String& params) {

}

String ActionToServerBridge::toHexString(uint8_t* data, int len) {
	String result;
	for(int t = 0; t < len; t++) {
		uint8_t b = (data[t] >> 4) & 0x0F;
		char c = (b < 0xA) ? '0' + b : 'A' + b - 0xA;
		result.concat(c);

		b = data[t] & 0x0F;
		c = (b < 0xA) ? '0' + b : 'A' + b - 0xA;
		result.concat(c);
	}
	return result;
}
