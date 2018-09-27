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

 DisplayMgr.h
 Created on: Sep 1, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef DisplayMgr_hpp
#define DisplayMgr_hpp

#include <SSD1306.h>

enum DisplayMgrMode {
  DISPL_WAIT_FOR_CON = 0, //wait for connect to AP
  DISPL_NORMAL = 1, //normal state of work
  DISPL_CONFIG, //enabled configuration screens
  DISPL_AP_CONFIG, //enabled AccessPoint name & password configuration
};

class DisplayMgr {
  public:
    void begin();
    void update();
    void setMode(DisplayMgrMode mode) { turnOnLCD(); this->mode = mode; }
    DisplayMgrMode getMode() { return mode; }
    void showText(String line1, String line2);
    void turnOffLCD();
    void turnOnLCD();
  private:
    DisplayMgrMode mode;
    unsigned long outTime = 0;
    uint8_t animFrame = 0;
    String lines[2];
    bool isDisplayOn = false;

    void normalMode();
    void configMode();
    void waitForWifiMode();
    void configureAPMode();
};

extern DisplayMgr displayMgr;
#endif /* DisplayMgr_hpp */
