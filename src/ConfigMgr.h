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

 ConfigMgr.h
 Created on: Sep 2, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#ifndef ConfigMgr_hpp
#define ConfigMgr_hpp

#include <Arduino.h>
#include <executables/Executable.h>
#include <vector>
#include <functional>

class ConfigMgr : public Executable {
  public:
    uint8_t page;

    virtual ~ConfigMgr() {}

    virtual bool execute() override;
    virtual void serialize(File& file) override {};
    void end();
  private:
    std::vector<Executable*> actions;

    Executable* addLambda(std::function<bool()> lambda);
    void runSelectedAction();
    void updateDisplayedPage();
    void showUsedKeys();
    void releaseAllActions();
    void clearUsedKeys();
    void factoryReset();
    void confirmableAction(String line1, String line2,
        std::function<void()> execOnConfirm, bool backToConfig = true);
};

extern ConfigMgr configMgr;

#endif /* ConfigMgr_hpp */
