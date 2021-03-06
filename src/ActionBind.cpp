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

 ActionBind.cpp
 Created on: Aug 19, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <ActionBind.h>
#include "LedCtrl.h"
#include "display/DisplayMgr.h"
#include <network/ResponseParser.h>
#include <executables/HttpCommand.h>

static RgbColor LIME_COL(32, 128, 32);
static RgbColor RED_COL(128, 0, 0);

ActionBind::ActionBind(uint8_t buttonIndex, Executable* cmd)
: buttonIndex(buttonIndex), successColor(LIME_COL), failColor(RED_COL),
  parseResponse(false), lcdLine1(buttonIndex), lcdLine2(buttonIndex), cmd(cmd) {
}

ActionBind::ActionBind()
: buttonIndex(0), successColor(LIME_COL), failColor(RED_COL),
  parseResponse(false), lcdLine1(buttonIndex), lcdLine2(buttonIndex), cmd(nullptr) {
}

ActionBind::~ActionBind(){
  if (cmd) {
    delete cmd;
  }
}

bool ActionBind::execute() {
  displayMgr.showText(lcdLine1, lcdLine2);
  displayMgr.update();
  bool success = (cmd != nullptr) ? cmd->execute() : false;

  //TODO: remove this coupling
  if (cmd->getTypeId() == HTTP_COMMAND_TYPE_MARKER) {
    HttpCommand* hcmd = static_cast<HttpCommand*>(cmd);
    ResponseParser rp;
    rp.parse(hcmd->getResponse());
  }

  if (success) {
    ledCtrl.blinkPattern(buttonIndex, successPattern, successColor);
  } else {
    ledCtrl.blinkPattern(buttonIndex, failPattern, failColor);
  }
  return success;
}
