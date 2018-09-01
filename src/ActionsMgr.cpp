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

#define writePrimitive(file, val) file.write((const uint8_t*)&val, sizeof(val))
#define readPrimitive(file, val) file.read((uint8_t*)&val, sizeof(val))

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
    if (actions[act->buttonIndex] != nullptr) {
      delete actions[act->buttonIndex];
    }
    actions[act->buttonIndex] = act;
    buttons.setButtonFunction(act->buttonIndex, act, nullptr);
//TODO:enable    presistAction(act);
  }
}

void ActionsMgr::presistAction(ActionBind* action) {
  FILENAME(path, action->buttonIndex);
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

void ActionsMgr::removeFromPersistance(uint8_t index) {
  FILENAME(path, index);
  SPIFFS.remove(path);
}

void ActionsMgr::loadActions() {
  FILENAME_TYPE(path);
  for(int t = 0; t < 8; t++) {
    if (actions[t] != nullptr) {
      delete actions[t];
      actions[t] = nullptr;
      buttons.setButtonFunction(t, nullptr, nullptr);
    }
    SET_FILENAME(path, t);
    File f = SPIFFS.open(path, "r");
    if (f) {
#ifdef LOG_ENABLED
      if (actions[t]->buttonIndex != t) {
        Serial.print("Action has wrong button index, expected:");
        Serial.print(t);
        Serial.print(" but is:);");
        Serial.print(actions[t]->buttonIndex);
      }
#endif
      actions[t] = loadAction(f);
      f.close();
      buttons.setButtonFunction(t, actions[t], nullptr);
    }
  }
}

ActionBind* ActionsMgr::loadAction(File& fileIn) {
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

  uint8_t isExecutable;
  readPrimitive(fileIn, isExecutable);
  if (isExecutable != 0) {
    //TODO: rozparsowac rozne typy
  }
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

  uint8_t isExecutable = action->cmd != nullptr ? 1 : 0;
  writePrimitive(fileOut, isExecutable);
  if (isExecutable != 0) {
    action->cmd->serialize(fileOut);
  }
}

void ActionsMgr::readString(File& file, String& str) {
  int len;
  readPrimitive(file, len);
  str.reserve(len);
  char buf[10];
  while(len > 0) {
    size_t r = file.read((uint8_t*)buf, len >= 10 ? 10 : len);
    //OMG... my eyes are bleeding
    for(size_t t = 0; t < r; t++) {
      str.concat(buf[t]);
    }
    len -= r;
  }
}

void ActionsMgr::writeString(File& file, String& str) {
  int len = str.length();
  writePrimitive(file, len);
  file.write((const uint8_t*)str.c_str(), len);
}
