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
#include <network/MyServer.h>
#include "BatteryMonitor.h"

SSD1306  display(0x3c, 5, 4);
BatteryMonitor batteryMonitor;

void DisplayMgr::begin() {
  display.init();
  display.displayOn();
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
  display.clear();
  switch(mode) {
    case DISPL_CONFIG:
      configMode();
      break;

    case DISPL_WAIT_FOR_CON:
      waitForWifiMode();
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
  //TODO: Status bar

  display.setFont(ArialMT_Plain_24);
  //TODO: Main area

  batteryMonitor.drawAt(display, 1, 1);

  display.display();
}

void DisplayMgr::waitForWifiMode() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 20, "Czekaj");
  batteryMonitor.drawAt(display, 1, 1);
  display.display();
}

void DisplayMgr::configMode() {
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Konfiguracja");
  display.drawString(0, 16, myServer.getServerIp());
  display.drawString(0, 37, myServer.getPassword());
  display.display();
  myServer.update();
}

void DisplayMgr::setMode(DisplayMgrMode mode) {
  this->mode = mode;
}
