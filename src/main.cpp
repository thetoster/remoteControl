#include <Arduino.h>
#include <Wire.h>
#include <stdlib.h>
#include <SSD1306.h>

const String versionString {"0.0.1"};

SSD1306  display(0x3c, 5, 4);

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
}

void loop() {
}
