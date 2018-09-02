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

#define XBM_NET_WIFI_WIDTH 16
#define XBM_NET_WIFI_HEIGHT 16
static unsigned char NET_WIFI_XBM[] = {
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x0f, 0x08, 0x10, 0xe4, 0x27, 0x10, 0x08,
   0xc0, 0x03, 0x20, 0x04, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01,
   0xc0, 0x03, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00 };

#define XBM_ACCESS_POINT_WIDTH 16
#define XBM_ACCESS_POINT_HEIGHT 16
static unsigned char ACCESS_POINT_XBM[] = {
  0xe0, 0x07, 0xf0, 0x0f, 0x3c, 0x3c, 0xce, 0x73, 0xe6, 0x67, 0x73, 0xce,
  0x3b, 0xdc, 0x9b, 0xd9, 0x9b, 0xd9, 0x3b, 0xdc, 0x73, 0xce, 0x66, 0x66,
  0x0e, 0x70, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x00, };

#define XBM_LEFT_WIDTH 16
#define XBM_LEFT_HEIGHT 16
static unsigned char LEFT_XBM[] = {
0x00, 0x00, 0xe0, 0x7f, 0x20, 0x40, 0x20, 0x44, 0x20, 0x46, 0x28, 0x45,
0xac, 0x44, 0x2e, 0x44, 0x2f, 0x44, 0x2e, 0x44, 0x2c, 0x44, 0x28, 0x44,
0x20, 0x4f, 0x20, 0x40, 0xe0, 0x7f, 0x00, 0x00};

#define XBM_RIGHT_WIDTH 16
#define XBM_RIGHT_HEIGHT 16
static unsigned char RIGHT_XBM[] = {
0x00, 0x00, 0xfe, 0x07, 0x02, 0x04, 0xf2, 0x04, 0x0a, 0x15, 0x0a, 0x35,
0x82, 0x74, 0x42, 0xf4, 0x22, 0x74, 0x22, 0x34, 0x12, 0x14, 0x0a, 0x04,
0xfa, 0x05, 0x02, 0x04, 0xfe, 0x07, 0x00, 0x00};

#define XBM_SELECT_WIDTH 16
#define XBM_SELECT_HEIGHT 16
static unsigned char SELECT_XBM[] = {
0x00, 0x00, 0xff, 0x03, 0x01, 0x02, 0x39, 0x02, 0x45, 0x02, 0x41, 0x02,
0x41, 0x02, 0x31, 0x82, 0x41, 0x42, 0x41, 0x2a, 0x45, 0x12, 0x45, 0x02,
0x39, 0x02, 0x01, 0x02, 0xff, 0x03, 0x00, 0x00};

SSD1306  display(0x3c, 5, 4);
BatteryMonitor batteryMonitor;

#define PRESENTATION_TIME (4 * 1000)

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
  display.drawXbm(127 - XBM_NET_WIFI_WIDTH, 0,
      XBM_NET_WIFI_WIDTH, XBM_NET_WIFI_HEIGHT,
      NET_WIFI_XBM);
  display.display();
}

void DisplayMgr::waitForWifiMode() {
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 20, "Czekaj");
  batteryMonitor.drawAt(display, 1, 1);
  if (animFrame == 0) {
    display.drawXbm(127 - XBM_NET_WIFI_WIDTH, 0,
          XBM_NET_WIFI_WIDTH, XBM_NET_WIFI_HEIGHT,
          NET_WIFI_XBM);
  }
  if ((millis() > outTime) or (outTime == (unsigned long)-1)) {
    animFrame = animFrame == 0 ? 1 : 0;
    outTime = millis() + 500;
  }
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
  //Access point icon
  /*
  display.drawXbm(127 - XBM_ACCESS_POINT_WIDTH, 0,
      XBM_ACCESS_POINT_WIDTH, XBM_ACCESS_POINT_HEIGHT,
      ACCESS_POINT_XBM);
*/
  display.display();
}

void DisplayMgr::setMode(DisplayMgrMode mode) {
  this->mode = mode;
}

void DisplayMgr::showText(String line1, String line2) {
  lines[0] = line1;
  lines[1] = line2;
  outTime = (line1.length() + line2.length()) > 0 ? millis() + PRESENTATION_TIME
      : (unsigned long)-1;
}
