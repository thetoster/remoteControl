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

 NetworkCtrl.cpp
 Created on: Sep 27, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#include "NetworkCtrl.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <display/DisplayMgr.h>
#include <Prefs.h>
#include "debug.h"
#include <numeric>
#include <SleepMgr.h>

NetworkCtrl networkCtrl;

void NetworkCtrl::putNetworkDown() {
  LOG_LN("NetCtrl: Put net down");
  WiFi.softAPdisconnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.setAutoConnect(false);
  WiFi.disconnect(false);
  WiFi.enableAP(false);
  WiFi.enableSTA(false);

  delay(1000);
}

void NetworkCtrl::connectToAccessPoint() {
  /* When your router is badass motherfucker...*/
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);

  WiFi.softAPdisconnect(false);
  WiFi.enableAP(false);
  WiFi.begin(prefs.storage.ssid, prefs.storage.wifiPassword);
  WiFi.setAutoReconnect(true);
  WiFi.setAutoConnect(true);
  startConnectMillis = millis();
  LOG("NetCtrl: Connect to access point:'");
  LOG(prefs.storage.ssid);
  LOG_LN("'");
//Dangerous, beware!
//  LOG("password:'");
//  LOG(prefs.storage.wifiPassword);
//  LOG_LN("'");
}

void NetworkCtrl::enableSoftAP() {
  LOG_LN("NetCtrl: Enabling softAP");
  WiFi.softAP(prefs.storage.inNetworkName, prefs.storage.password);
}

void NetworkCtrl::update() {
  if (startConnectMillis != 0) {
    if (WiFi.status() == WL_CONNECTED) {
      unsigned long delta = millis() - startConnectMillis;
      delta += 1000;  //add one second as a jitter
      startConnectMillis = 0;
      LOG("NetCtrl: measured connection time ");
      LOG(delta);
      LOG_LN(" ms");

      //if tool long, then don't bother
      if (delta < 30*1000) {
        times.push_back(delta);
        if (times.size() > 5) {
          times.erase(times.begin());
        }
        LOG("NetCtrl: new wait for connection timeout is ");
        LOG(calcTimeout());
        LOG_LN(" ms");
      }
    }
  }
}

unsigned long NetworkCtrl::calcTimeout() {
  if (times.size() > 0) {
    unsigned long sum = std::accumulate(times.begin(), times.end(), 0);
    return sum / times.size();

  } else {
    return 10000;
  }
}

bool NetworkCtrl::waitForAPConnection() {
  auto timeoutTime = calcTimeout();
  LOG("NetCtrl: waitForAPConnection, timeout is ");
  LOG(timeoutTime);
  LOG(" ms...");

  sleepMgr.stimulate(timeoutTime);
  timeoutTime += millis();
  while (WiFi.status() != WL_CONNECTED) {
    displayMgr.update();
    delay(25);
    if (millis() > timeoutTime) {
      LOG_LN(" CONNECTION NOT READY");
      return false;
    }
  }
  LOG_LN(" CONNECTED");
  return true;
}
