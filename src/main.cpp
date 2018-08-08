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

const String versionString {"0.0.1"};

SSD1306  display(0x3c, 5, 4);

#include <NeoPixelBus.h>
#define colorSaturation 128
#define PIN_LED_DATA 13
NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> strip(8, PIN_LED_DATA);


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
A0
  strip.Begin();
  strip.Show();
  RgbColor red(0, colorSaturation, 0);
  for(int t = 0; t < 8; t++) {
    strip.SetPixelColor(t, red);
  }
  strip.Show();
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
  Serial.println("test");
  delay(2000);

}
