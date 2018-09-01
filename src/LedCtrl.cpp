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

 LedCtrl.cpp
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include "LedCtrl.h"

#define MAX_COLOR_SATURATION 128
#define PIN_LED_DATA 13
#define SHORT_TIME (100)
#define LONG_TIME (250)

static NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> strip(8, PIN_LED_DATA);
static RgbColor BLACK_COL(0, 0, 0);
static const uint8_t WANTED_TO_HARDWARE[] = {7, 0, 6, 1, 5, 2, 4, 3};

void LedCtrl::begin() {
  strip.Begin();
  for(int t = 0; t < 8; t++) {
    strip.SetPixelColor(t, BLACK_COL);
  }
  strip.Show();
}

void LedCtrl::blinkPattern(int ledNo, String pattern, RgbColor color) {
  actions.push_back(LedContext(WANTED_TO_HARDWARE[ledNo], pattern, color));
}

void LedCtrl::blinkError(int ledNo) {
  actions.push_back(LedContext(WANTED_TO_HARDWARE[ledNo], "*.*.*.*.*", RgbColor(128, 0, 0) ) );
}

void LedCtrl::update() {
  uint8_t usedLeds = 0;
  bool isChange = false;
  for(std::list<LedContext>::iterator it = actions.begin(); it != actions.end(); it++) {
    if (bitRead(usedLeds, it->ledIndex) == 0) {
      bitSet(usedLeds, it->ledIndex);
      if (handleLedContext(*it, isChange) ) {
        it = actions.erase(it);
      }
    }
  }

  if (isChange) {
    strip.Show();
  }
}

bool LedCtrl::handleLedContext(LedContext& ctx, bool& isChange) {
  long curMillis = millis();
  if (ctx.changeMillis < curMillis) {
    ctx.patternIndex ++;
    isChange = true;
    if (ctx.pattern.length() <= static_cast<size_t>(ctx.patternIndex)) {
      strip.SetPixelColor(ctx.ledIndex, BLACK_COL);
      return true;
    }

    switch(ctx.pattern.charAt(ctx.patternIndex)) {
      case '.': //short off
        strip.SetPixelColor(ctx.ledIndex, BLACK_COL);
        ctx.changeMillis = curMillis + SHORT_TIME;
        break;

      case '*': //short color
        strip.SetPixelColor(ctx.ledIndex, ctx.color);
        ctx.changeMillis = curMillis + SHORT_TIME;
        break;

      case '_': //long off
        strip.SetPixelColor(ctx.ledIndex, BLACK_COL);
        ctx.changeMillis = curMillis + LONG_TIME;
        break;

      case '-': //long color
        strip.SetPixelColor(ctx.ledIndex, ctx.color);
        ctx.changeMillis = curMillis + LONG_TIME;
        break;
    }
  }
  return false;
}
