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

 Buttons.cpp
 Created on: Aug 9, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <Buttons.h>
#include <Arduino.h>
#include <SleepMgr.h>

static const uint8_t WANTED_TO_HARDWARE[] = {7, 3, 6, 2, 5, 1, 4, 0};

void Buttons::begin() {
  if (pcf20) {
    return;
  }
  pcf20 = new PCF8574(0x20);
  pcf20->begin();
  for(int t = 0; t < 8; t++) {
    buttons[t].lastPressed = false;
    buttons[t].longPressCallback = nullptr;
    buttons[t].shortPressCallback = nullptr;
    buttons[t].pressedTickCount = 0;
  }
}

void Buttons::getButtonFunction(uint8_t index, Executable* &shortPress, Executable* &longPress) {
  if (index < 8) {
    index = WANTED_TO_HARDWARE[index];
    shortPress = buttons[index].shortPressCallback;
    longPress = buttons[index].longPressCallback;
  }
}

void Buttons::setButtonFunction(uint8_t index, Executable* shortPress, Executable* longPress) {
  if (index < 8) {
    index = WANTED_TO_HARDWARE[index];
    buttons[index].longPressCallback = longPress;
    buttons[index].shortPressCallback = shortPress;
  }
}

void Buttons::update() {
  if (millis() - lastMilis > BTN_READ_DELAY) {
    lastMilis = millis();

    uint8_t state = pcf20->read8();
    bool stimuli = false;
    for(int t = 0; t < 8; t++) {
      //note inverted state! Button pressed is 0 not 1
      bool r = handleButtonUpdate(buttons[t], bitRead(state, t) == 0);
      stimuli |= r;
    }
    if (stimuli) {
      sleepMgr.stimulate();
    }
  }
}

bool Buttons::handleButtonUpdate(ButtonContext& ctx, bool isPressed) {
  bool somethingChanged = false;

  if (ctx.lastPressed && isPressed) {
    //user continue to hold pressed button
    ctx.pressedTickCount ++;
    ctx.pressedTickCount = ctx.pressedTickCount < 255 ? ctx.pressedTickCount : 254;

  } else if ( (not ctx.lastPressed) and isPressed) {
    //user just pressed button
    ctx.lastPressed = true;
    ctx.pressedTickCount = 1;
    somethingChanged = true;

  } else if ( ctx.lastPressed and (not isPressed)) {
    //user just released button
    Executable* toExec = (ctx.pressedTickCount > BTN_LONG_PRESS_TICKS) ?
        ctx.longPressCallback : ctx.shortPressCallback;

    if (toExec) {
      toExec->execute();
    }

    ctx.lastPressed = false;
    ctx.pressedTickCount = 0;
    somethingChanged = true;
  }

  return somethingChanged;
}
