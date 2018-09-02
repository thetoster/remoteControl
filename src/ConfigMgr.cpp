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

 ConfigMgr.cpp
 Created on: Sep 2, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */
#include <ConfigMgr.h>
#include <executables/LambdaExecutable.h>
#include <Buttons.h>
#include <display/DisplayMgr.h>
#include <LedCtrl.h>
#include <ActionsMgr.h>

static RgbColor NAVI_KEY_COL(32, 32, 128);
static RgbColor SELECT_KEY_COL(32, 128, 32);
static RgbColor QUIT_KEY_COL(128, 32, 32);

bool ConfigMgr::execute() {
  Serial.println("----CONFIG");
  for(int t = 0; t < 8; t++) {
    buttons.setButtonFunction(t, nullptr, nullptr);
  }

  //prev on button 0
  buttons.setButtonFunction(0, addLambda([this](){
    if (this->page > 0) {
      this->page --;
    } else {
      this->page = 4;
    }
    updateDisplayedPage();
    return true;
  }), nullptr);

  //next on buton 1
  buttons.setButtonFunction(1, addLambda([this](){
      if (this->page < 4) {
        this->page ++;
      } else {
        this->page = 0;
      }
      updateDisplayedPage();
      return true;
    }), nullptr);

  //next on buton 1
  buttons.setButtonFunction(2, addLambda([this](){
      runSelectedAction();
      return true;
    }), nullptr);

  //on last button exit
  buttons.setButtonFunction(7, addLambda([this](){
        end();
        return true;
      }), nullptr);

  page = 0;
  displayMgr.setMode(DISPL_CONFIG);
  updateDisplayedPage();

  //set colors
  ledCtrl.turnOn(0, NAVI_KEY_COL);
  ledCtrl.turnOn(1, NAVI_KEY_COL);
  ledCtrl.turnOn(2, SELECT_KEY_COL);
  ledCtrl.turnOn(7, QUIT_KEY_COL);

  return true;
}

void ConfigMgr::runSelectedAction() {

}

void ConfigMgr::updateDisplayedPage() {
  switch(page) {
    case 0:
      displayMgr.showText("Uruchom server", "konfiguracji");
      break;
    case 1:
      displayMgr.showText("Pokaz przypisane", "przyciski");
      break;
    case 2:
      displayMgr.showText("Wyczysc", "przypisania");
      break;
    case 3:
      displayMgr.showText("Ustawienia", "fabryczne");
      break;
    case 4:
      displayMgr.showText("Zamknij", "konfiguracje");
      break;
  }
}

void ConfigMgr::end() {
  ledCtrl.turnOffAll();
  displayMgr.setMode(DISPL_NORMAL);
  actionsMgr.loadActions();
}

Executable* ConfigMgr::addLambda(std::function<bool()> lambda) {
  Executable* result = new LambdaExecutable(lambda);
  actions.push_back(result);
  return result;
}
