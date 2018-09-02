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

 LedCtrl.h
 Created on: Aug 8, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef LedCtrl_hpp
#define LedCtrl_hpp

#include <Arduino.h>
#include <list>
#include <NeoPixelBus.h>

class LedCtrl {

  public:
    void begin();
    void blinkError(int ledNo);
    void blinkPattern(int ledNo, String pattern, RgbColor color);
    void turnOn(int ledNo, RgbColor color);
    void turnOff(int ledNo);
    void turnOffAll();
    void update();
  private:
    struct LedContext {
      RgbColor color;
      String pattern;
      long changeMillis = 0;
      int8_t patternIndex = -1;
      uint8_t ledIndex;

      LedContext(uint8_t ledIndex, String pattern, RgbColor color)
        : color(color), pattern(pattern), ledIndex(ledIndex) {}
    };
    std::list<LedContext> actions;
    bool handleLedContext(LedContext& ctx, bool& isChange);
};

extern LedCtrl ledCtrl;
#endif /* LedCtrl_hpp */
