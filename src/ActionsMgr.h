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

 ActionsMgr.h
 Created on: Aug 27, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef ActionsMgr_hpp
#define ActionsMgr_hpp

#include <FS.h>

class ActionBind;
class ActionsMgr {
  public:
    ActionBind* actions[8];

    void begin();
    void loadActions();
    void putAction(ActionBind* act);
    void removeAction(uint8_t index);
    void clear();
  private:

    ActionBind* loadAction(File& fileIn);
    void saveAction(File& fileIn, ActionBind* action);

    void presistAction(ActionBind* action);
    void removeFromPersistance(uint8_t index);
};

extern ActionsMgr actionsMgr;
#endif /* ActionsMgr_hpp */
