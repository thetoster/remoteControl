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

 ActionsMgr.cpp
 Created on: Aug 27, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#include <ActionsMgr.h>
#include "ActionBind.h"
#include "Buttons.h"
#include "FileHelper.h"
#include "executables/HttpCommand.h"
#include "ConfigMgr.h"
#include "debug.h"

//filenames are: /A, /B... etc
#define FILENAME_TYPE(path) char path[3]; path[0] = '/'; path[2] = 0
#define FILENAME(path, index) char path[] = {'/', (char)(65 + index), 0}
#define SET_FILENAME(path, index) path[1] = (char)(65 + index)

void ActionsMgr::begin() {
  SPIFFS.begin();
  for(int t = 0; t < 8; t++) {
    actions[t] = nullptr;
  }
}

void ActionsMgr::putAction(ActionBind* act) {
  if (act != nullptr) {
    LOG("Put action for key:");
    LOG_LN(act->buttonIndex);
    if (actions[act->buttonIndex] != nullptr) {
      delete actions[act->buttonIndex];
    }
    actions[act->buttonIndex] = act;
    if (act->buttonIndex == 7 ) {
      buttons.setButtonFunction(act->buttonIndex, act, &configMgr);
    } else {
      buttons.setButtonFunction(act->buttonIndex, act, nullptr);
    }
    presistAction(act);
  }
}

void ActionsMgr::presistAction(ActionBind* action) {
  FILENAME(path, action->buttonIndex);
  LOG("Save action ");
  LOG(action->buttonIndex);
  LOG(" -> ");
  LOG_LN(path);
  File f = SPIFFS.open(path, "w");
  saveAction(f, action);
  f.close();
}

void ActionsMgr::removeAction(uint8_t index) {
  if (actions[index] != nullptr) {
    delete actions[index];
    actions[index] = nullptr;
    buttons.setButtonFunction(index, nullptr, nullptr);
    removeFromPersistance(index);
  }
}
void ActionsMgr::clear() {
  for(int t = 0; t < 8; t++) {
    removeAction(t);
  }
  buttons.setButtonFunction(7, nullptr, &configMgr);
}

void ActionsMgr::removeFromPersistance(uint8_t index) {
  FILENAME(path, index);
  bool b = SPIFFS.remove(path);
  LOG("Remove file:");
  LOG(path);
  LOG(", result=");
  LOG_LN(b);
}

void ActionsMgr::loadActions() {
  //make things crash ;(
  FILENAME_TYPE(path);
  for(int t = 0; t < 8; t++) {
    if (actions[t] != nullptr) {
      delete actions[t];
      actions[t] = nullptr;
      buttons.setButtonFunction(t, nullptr, nullptr);
    }
    SET_FILENAME(path, t);
    LOG_LN(path);

    File f = SPIFFS.open(path, "r");
    if (f and (f.size() > 0)) {
      actions[t] = loadAction(f);
      if (actions[t]->buttonIndex != t) {
        LOG("Action has wrong button index, expected:");
        LOG(t);
        LOG(" but is:);");
        LOG_LN(actions[t]->buttonIndex);
      }
      buttons.setButtonFunction(t, actions[t], nullptr);
    	LOG_LN("OPEN OK");
    }
    f.close();
  }
  LOG_LN("Load actions end =====");

  //longpress on button 7 always run config
  Executable* tmp;
  Executable* tmp2;
  buttons.getButtonFunction(7, tmp, tmp2);
  buttons.setButtonFunction(7, tmp, &configMgr);
}

ActionBind* ActionsMgr::loadAction(File& fileIn) {
  LOG_LN("Load->");
  ActionBind* action = new ActionBind();
  readPrimitive(fileIn, action->buttonIndex);

  readString(fileIn, action->successPattern);
  readPrimitive(fileIn, action->successColor.R);
  readPrimitive(fileIn, action->successColor.G);
  readPrimitive(fileIn, action->successColor.B);

  readString(fileIn, action->failPattern);
  readPrimitive(fileIn, action->failColor.R);
  readPrimitive(fileIn, action->failColor.G);
  readPrimitive(fileIn, action->failColor.B);

  readPrimitive(fileIn, action->parseResponse);

  readString(fileIn, action->lcdLine1);
  readString(fileIn, action->lcdLine2);

  uint8_t execType;
  readPrimitive(fileIn, execType);

  if (execType == HTTP_COMMAND_TYPE_MARKER) {
    action->cmd = new HttpCommand(fileIn);

  } else {
    LOG("Unknown serialized type:");
    LOG_LN(execType);
  }

  LOG_LN("<-end");
  return action;
}

void ActionsMgr::saveAction(File& fileOut, ActionBind* action) {
  writePrimitive(fileOut, action->buttonIndex);

  writeString(fileOut, action->successPattern);
  writePrimitive(fileOut, action->successColor.R);
  writePrimitive(fileOut, action->successColor.G);
  writePrimitive(fileOut, action->successColor.B);

  writeString(fileOut, action->failPattern);
  writePrimitive(fileOut, action->failColor.R);
  writePrimitive(fileOut, action->failColor.G);
  writePrimitive(fileOut, action->failColor.B);

  writePrimitive(fileOut, action->parseResponse);

  writeString(fileOut, action->lcdLine1);
  writeString(fileOut, action->lcdLine2);

  uint8_t execType = action->cmd == nullptr ? 0 : action->cmd->getTypeId();
  writePrimitive(fileOut, execType);
  if (execType != 0) {
    action->cmd->serialize(fileOut);
  }
}
