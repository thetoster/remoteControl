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

 SleepMgr.cpp
 Created on: Sep 27, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#include "SleepMgr.h"
#include <Arduino.h>
#include <display/DisplayMgr.h>
#include <ESP8266WiFi.h>
#include <network/MyServer.h>
#include <Prefs.h>
#include <network/NetworkCtrl.h>
#include "debug.h"
#include <LedCtrl.h>

#define PROLONG_TIME  (15000)

SleepMgr sleepMgr;

void SleepMgr::update() {
  if ((not enabled) or (millisToSleep == 0)) {
    return;
  }
  if (lastMillis == 0) {
    lastMillis = millis();
    return;
  }

  unsigned long delta = (millis() - lastMillis);
  lastMillis = millis();
  if (millisToSleep > delta) {
    millisToSleep -= delta;
  } else {
    millisToSleep = 0;
  }

  if (millisToSleep == 0) {
    //go sleep
    LOG_LN("SleepMgr: Go to sleep");
    displayMgr.turnOffLCD();
    networkCtrl.putNetworkDown();
    ledCtrl.turnOffAll();
  }
}

void SleepMgr::stimulate() {
  LOG_LN("SleepMgr: stimulate");
  millisToSleep += PROLONG_TIME;
  if ((WiFi.getMode() == WIFI_OFF) and prefs.hasPrefs()) {
    networkCtrl.connectToAccessPoint();
    LOG_LN("SleepMgr: reconnect network");
  }
}

void SleepMgr::enable() {
  enabled = true;
  LOG_LN("SleepMgr: enabled");
}

void SleepMgr::disable() {
  enabled = false;
  LOG_LN("SleepMgr: disabled");
}
