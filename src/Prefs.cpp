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

 Prefs.cpp
 Created on: Dec 27, 2017
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <Prefs.h>
#include <EEPROM.h>

Prefs prefs;

void Prefs::load() {
  EEPROM.begin(512);
  EEPROM.get(0, storage);
  EEPROM.end();
  uint8_t expectedCrc = calcCRC();

  Serial.print("StorageCrc:");
  Serial.println(storage.crc);
  Serial.print("Expected CRC:");
  Serial.println(expectedCrc);
  Serial.print("Is zero prefs:");
  Serial.println(isZeroPrefs());
  Serial.flush();

  if (not hasPrefs()) {
    defaultValues();
  }
}

bool Prefs::hasPrefs() {
  return (storage.crc == calcCRC()) && (not isZeroPrefs()) && (prefs.storage.ssid[0] != 0);
}

bool Prefs::isZeroPrefs() {
  const uint8_t* data = (uint8_t*)&storage;
  for(size_t t = 0; t < sizeof(storage); t++) {
    if (*data != 0) {
      return false;
    }
    data++;
  }
  return true;
}

void Prefs::defaultValues() {
  Serial.println("Reset prefs to default");

  memset(&storage, 0, sizeof(storage));
  strcpy(&storage.inNetworkName[0], "RemoteCtrl");
}

void Prefs::save() {
  storage.crc = calcCRC();
  EEPROM.begin(512);
  EEPROM.put(0, storage);
  EEPROM.commit();
  EEPROM.end();
}

uint8_t Prefs::calcCRC() {
  const uint8_t* data = (uint8_t*)&storage;
  data++; //skip crc field
  int len = sizeof(storage) - 1;
  uint8_t crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}
