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

 BatteryMonitor.cpp
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <Arduino.h>
#include <BatteryMonitor.h>

BatteryMonitor::BatteryMonitor() : blink(true) {
  pinMode(A0, INPUT);
}

void BatteryMonitor::drawAt(SSD1306& disp, int x, int y, int w, int h) {
  int batLevel = calcBatteryLevel();
  blink = (batLevel <= batteryLowWarningLevel) ? not blink : true;
  if (blink) {
    disp.drawRect(x, y, w, h);
  }
  h -= 4;
  w -= 4;
  w /= levels;
  w -= 2;
  y += 2;

  for(; batLevel > 0; batLevel--) {
    x += 2;
    disp.fillRect(x, y, w , h);
    x += w + 2;
  }

}

int BatteryMonitor::calcBatteryLevel() {
  int reading = analogRead(A0);
  int levelRange = (maxBattery - minBattery) / levels;
  int level = (reading - minBattery) / levelRange;
  level = level < 0 ? 0 : level;
  level = level >= levels ? levels -1 : level;
  return level;
}
