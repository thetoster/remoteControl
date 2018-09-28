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

 FileHelper.cpp
 Created on: Sep 1, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <FileHelper.h>

void readString(File& file, String& str) {
  int len;
  readPrimitive(file, len);
  str.remove(0);
  str.reserve(len);
  char buf[10];
  while(len > 0) {
    size_t r = file.read((uint8_t*)buf, len >= 10 ? 10 : len);
    //OMG... my eyes are bleeding
    for(size_t t = 0; t < r; t++) {
      str.concat(buf[t]);
    }
    len -= r;
  }
}

void writeString(File& file, String& str) {
  int len = str.length();
  writePrimitive(file, len);
  file.write((const uint8_t*)str.c_str(), len);
}

