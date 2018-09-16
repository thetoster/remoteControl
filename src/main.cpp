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
*/

#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <sha256.h>
#include "LedCtrl.h"
#include "Buttons.h"
#include "ActionsMgr.h"
#include "ActionBind.h"
#include "ConfigMgr.h"
#include "network/MyServer.h"
#include "network/Updater.h"
#include "display/DisplayMgr.h"
#include "executables/HttpCommand.h"
#include "executables/ShowTextExecutable.h"
#include <ESP8266WiFi.h>
#include <Prefs.h>

const String versionString {"0.0.1"};

DisplayMgr displayMgr;
Buttons buttons;
LedCtrl ledCtrl;
ActionsMgr actionsMgr;
ConfigMgr configMgr;
wl_status_t lastWifiStatus;

/* Indices of Buttons and Leds are made in this way just to confuse Russians...
Wanted order is:
  1 2
  3 4
  5 6
  7 8

Hardware order is:
  BTN   LED
  7 3   7 0
  6 2   6 1
  5 1   5 2
  4 0   4 3
 */

void setDebugActions() {
  uint8_t pass[] = {1,2,3,4};
  HttpCommand* cmd = new HttpCommand("http://ptsv2.com/t/5kpry-1533759192/post", pass, 4, false);
  cmd->addData("dupa", "blada");

  ActionBind* act = new ActionBind();
  act->buttonIndex = 0;
  act->cmd = cmd;
  act->lcdLine1 = "Action 1";
  act->lcdLine2 = "";
  actionsMgr.putAction(act);

  ///
  cmd = new HttpCommand("1533759192/post", pass, 4, false);
  act = new ActionBind();
  act->buttonIndex = 1;
  act->cmd = cmd;
  act->lcdLine1 = "Action 2";
  act->lcdLine2 = "Red fail";
  actionsMgr.putAction(act);
}

void setup() {
#if LOG_ENABLED==1
  Serial.begin(115200);
#endif
  displayMgr.begin();
  ledCtrl.begin();
  buttons.begin();
  actionsMgr.begin();

  //debug ----
  setDebugActions();
  lastWifiStatus = WL_DISCONNECTED;
  //------^

  if (not prefs.hasPrefs()) {
    displayMgr.setMode(DISPL_AP_CONFIG);
    myServer.begin();

  } else {
    actionsMgr.loadActions();
    displayMgr.setMode(DISPL_WAIT_FOR_CON);
  }
}

void loop() {
  if (updater.update()) {
    return;
  }

  myServer.update();
  displayMgr.update();
  buttons.update();
  ledCtrl.update();
  delay(25);
}
