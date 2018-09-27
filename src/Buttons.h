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

 Buttons.h
 Created on: Aug 9, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef Buttons_hpp
#define Buttons_hpp

#include <PCF8574.h>
#include <Wire.h>
#include "executables/Executable.h"

#define BTN_READ_DELAY (50)
//time in ms after which long press is recognized
#define BTN_LONG_PRESS_TICKS (1500 / BTN_READ_DELAY)

class Buttons {
  public:
    Buttons() {};
    void begin();
    void update();
    void setButtonFunction(uint8_t index, Executable* shortPress, Executable* longPress);
    void getButtonFunction(uint8_t index, Executable* &shortPress, Executable* &longPress);
  private:
    struct ButtonContext {
            Executable* shortPressCallback = nullptr;
            Executable* longPressCallback = nullptr;
            bool lastPressed;
            uint8_t pressedTickCount;
        };

    PCF8574* pcf20 = nullptr;
    long lastMilis = 0;
    ButtonContext buttons[8];

    bool handleButtonUpdate(ButtonContext& ctx, bool isPressed);
};

extern Buttons buttons;
#endif /* Buttons_hpp */
