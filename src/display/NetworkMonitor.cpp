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

 NetworkMonitor.cpp
 Created on: Sep 24, 2018
 Author: Bartłomiej Żarnowski (Toster)
 */

#include "NetworkMonitor.h"
#include "XbmIcons.h"
#include "debug.h"

#define FAST_BLINK_MS (250)
#define SLOW_BLINK_MS (500)

void NetworkMonitor::drawAt(SSD1306& display, int x, int y) {

	if (lastWifiMode != WiFi.getMode()) {
		outTime = 0;
		shown = false;
		lastWifiMode = WiFi.getMode();
		LOG("NetworkMonitor: new mode=");
		LOG_LN(lastWifiMode);
	}

	switch(lastWifiMode) {
		default:
		case WIFI_OFF:
			return;

		case WIFI_STA:
			drawSTAStatus(display, x, y);
			break;

		case WIFI_AP:
		case WIFI_AP_STA:
			drawAPStatus(display, x, y);
			break;
	}
}

void NetworkMonitor::drawSTAStatus(SSD1306& display, int x, int y) {
	const wl_status_t status = WiFi.status();
	int deltaTime = 0;

	if (status == WL_CONNECTED) {
		//solid icon, no flashing
		deltaTime = 0;
		shown = true;

	} else if (status == WL_CONNECT_FAILED or status == WL_DISCONNECTED) {
		deltaTime = FAST_BLINK_MS;

	} else {
		deltaTime = SLOW_BLINK_MS;
	}

	if (deltaTime > 0) {
		if (millis() > outTime) {
			outTime = millis() + deltaTime;
			shown = not shown;
		}
	}

	if (shown) {
		display.drawXbm(x, y,
				          XBM_NET_WIFI_WIDTH, XBM_NET_WIFI_HEIGHT,
				          NET_WIFI_XBM);
	}
}

void NetworkMonitor::drawAPStatus(SSD1306& display, int x, int y) {
	if (millis() > outTime) {
		outTime = millis() + SLOW_BLINK_MS;
		shown = not shown;
	}

	if (shown) {
		display.drawXbm(x, y,
				XBM_ACCESS_POINT_WIDTH, XBM_ACCESS_POINT_HEIGHT,
				ACCESS_POINT_XBM);
	}
}
