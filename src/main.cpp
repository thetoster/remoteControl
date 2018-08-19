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
#include <SSD1306.h>
#include <sha256.h>
#include "MyServer.h"
#include "Updater.h"
#include "LedCtrl.h"
#include "HttpCommand.h"
#include "BatteryMonitor.h"
#include "ShowTextExecutable.h"
#include "Buttons.h"

const String versionString {"0.0.1"};

SSD1306  display(0x3c, 5, 4);
BatteryMonitor batteryMonitor;
Buttons buttons;
LedCtrl* ledCtrl;

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

void setup() {
  Serial.begin(115200);

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

  ledCtrl = new LedCtrl();
  buttons.begin();
}

void normalMode() {
  myServer.update();
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  //TODO: Status bar

  display.setFont(ArialMT_Plain_24);
  //TODO: Main area

  display.display();
  delay(200);
}

void configMode() {
  display.clear();
  display.setColor(WHITE);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Konfiguracja");
  display.drawString(0, 16, myServer.getServerIp());
  display.drawString(0, 37, myServer.getPassword());
  display.display();
  myServer.update();
  delay(200);
}

//bool used = false;
//void doDebugTest() {
//  if (used == false) {
//    used = true;
//    ledCtrl->blinkError(2);
//
//    ledCtrl->blinkPattern(3, String("-_-_*_*_*"), RgbColor(0,128,0));
//    ledCtrl->blinkPattern(4, String("-_-_*_*_*"), RgbColor(0,0,128));
//  }
//}

void loop() {
  /*
  if (updater.update()) {
    return;
  }

  //no update in progress
  if (myServer.isServerConfigured()) {
    normalMode();

  } else {
    configMode();
  }
  */

  display.clear();
  if (WiFi.status() == WL_CONNECTED) {
//    doDebugTest();
  } else {
    //display.clear();
    display.setColor(WHITE);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 20, "Waiting");
    //display.display();
  }

  buttons.update();
  batteryMonitor.drawAt(display, 1, 1);
  display.display();
  ledCtrl->update();
  delay(25);
}
