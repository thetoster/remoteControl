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

 ActionBind.h
 Created on: Aug 19, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef ActionBind_hpp
#define ActionBind_hpp

#include "executables/Executable.h"
#include <NeoPixelBus.h>

class ActionBind : public Executable {
  public:
    uint8_t buttonIndex;
    String successPattern = "--";
    RgbColor successColor;
    String failPattern = "*.*.*";
    RgbColor failColor;
    bool parseResponse; //will be used to work with LCD
    String lcdLine1;  //text to show on LCD (line 1)
    String lcdLine2;  //text to show on LCD (line 1)

    Executable* cmd;

    ActionBind();
    ActionBind(uint8_t buttonIndex, Executable* cmd);
    virtual ~ActionBind() override;

    virtual bool execute()  override;
    virtual void serialize(File& file) override {};
    uint8_t getTypeId() override {return cmd ? cmd->getTypeId() : 0;}
};

#endif /* ActionBind_hpp */
