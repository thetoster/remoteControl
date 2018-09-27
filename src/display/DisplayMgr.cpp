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

 DisplayMgr.cpp
 Created on: Sep 1, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <display/DisplayMgr.h>
#include <display/NetworkMonitor.h>
#include <network/MyServer.h>
#include "BatteryMonitor.h"
#include "XbmIcons.h"
#include <ESP8266WiFi.h>
#include <Prefs.h>

SSD1306  display(0x3c, 5, 4);
BatteryMonitor batteryMonitor;
NetworkMonitor networkMonitor;

#define PRESENTATION_TIME (4 * 1000)

void DisplayMgr::begin() {
  display.init();
  turnOnLCD();
  display.normalDisplay();
  display.setContrast(128);
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Bootowanie");
  display.drawString(0, 16, versionString);
  display.display();
}

void DisplayMgr::update() {
  if (not isDisplayOn) {
    return;
  }

  display.clear();
  switch(mode) {
    case DISPL_CONFIG:
      configMode();
      break;

    case DISPL_WAIT_FOR_CON:
      waitForWifiMode();
      break;

    case DISPL_AP_CONFIG:
      configureAPMode();
      break;

    default:
    case DISPL_NORMAL:
      normalMode();
      break;
  }
}

void DisplayMgr::normalMode() {
  myServer.update();
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  for(int t = 0; t < 2; t ++) {
    if (lines[t].length() > 0) {
      display.drawString(0, 20 + t * 20, lines[t]);
    }
  }
  if (millis() > outTime) {
    lines[0] = "";
    lines[1] = "";
    outTime = (unsigned long)-1;
  }
  batteryMonitor.drawAt(display, 1, 1);
  networkMonitor.drawAt(display, display.getWidth() - networkMonitor.getWidth(), 0);
  display.display();
}

void DisplayMgr::waitForWifiMode() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 20, "Czekaj");
  batteryMonitor.drawAt(display, 1, 1);
  networkMonitor.drawAt(display, display.getWidth() - networkMonitor.getWidth(), 0);
  display.display();
}

void DisplayMgr::configMode() {
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  display.drawXbm(0, 0, XBM_LEFT_WIDTH, XBM_LEFT_HEIGHT, LEFT_XBM);
  display.drawXbm(32, 0, XBM_SELECT_WIDTH, XBM_SELECT_HEIGHT, SELECT_XBM);
  display.drawXbm(64, 0, XBM_RIGHT_WIDTH, XBM_RIGHT_HEIGHT, RIGHT_XBM);

  display.setFont(ArialMT_Plain_16);
  for(int t = 0; t < 2; t ++) {
    if (lines[t].length() > 0) {
      display.drawString(0, 20 + t * 20, lines[t]);
    }
  }

  networkMonitor.drawAt(display, display.getWidth() - networkMonitor.getWidth(), 0);
  display.display();
}

void DisplayMgr::configureAPMode() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);

  //show name and IP
  if (prefs.storage.inNetworkName[0] != 0) {
    if (animFrame < 5) {
      display.drawString(0, 20, prefs.storage.inNetworkName);

    } else {
      display.drawString(0, 20, WiFi.softAPIP().toString());
    }

  } else {
    display.drawString(0, 20, WiFi.softAPIP().toString());
  }

  //Show password
  if (prefs.storage.password[0] == 0) {
    display.drawString(0, 40, "Do Factory R.");
  } else {
    display.drawString(0, 40, String(prefs.storage.password));
  }

  batteryMonitor.drawAt(display, 1, 1);
  networkMonitor.drawAt(display, display.getWidth() - networkMonitor.getWidth(), 0);

  if ((millis() > outTime) or (outTime == (unsigned long) -1)) {
    animFrame ++;
    if (animFrame > 10) {
      animFrame = 0;
    }
    outTime = millis() + 500;
  }
  display.display();
}

void DisplayMgr::showText(String line1, String line2) {
  lines[0] = line1;
  lines[1] = line2;
  outTime = (line1.length() + line2.length()) > 0 ? millis() + PRESENTATION_TIME
      : (unsigned long)-1;
  turnOnLCD();
}

void DisplayMgr::turnOffLCD() {
  if (isDisplayOn) {
    display.displayOff();
    isDisplayOn = false;
  }
}

void DisplayMgr::turnOnLCD() {
  if (not isDisplayOn) {
    display.displayOn();
    isDisplayOn = true;
  }
}
