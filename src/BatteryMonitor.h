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

 BatteryMonitor.h
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef BatteryMonitor_hpp
#define BatteryMonitor_hpp
#include <SSD1306.h>

class BatteryMonitor {
  public:
    uint16 batteryLowWarningLevel = 1;   //value in level units
    uint16 maxBattery = 900;   //for calibration ADC raw value 0..1024
    uint16 minBattery = 300;   //for calibration ADC raw value 0..1024
    uint8_t levels = 1; //how many bars should be drawn <0,levels)

    BatteryMonitor();
    void drawAt(SSD1306& disp, int x, int y, int w = 40, int h = 10);
    int calcBatteryLevel(); //value from 0..levels, where level 0 <= ADC for batteryLowWarning
  private:
    bool blink;
};

#endif /* BatteryMonitor_hpp */
