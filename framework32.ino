#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//#include <WiFiUdp.h>
#include <Wire.h>
//#define LONGCLICK_MS    500
//#define DOUBLECLICK_MS  500
#include <Button2.h>
#include <ArduinoJson.h>
#include "esp_adc_cal.h"
#include "util.h"
#include "configuration.h"
#include "App.h"
#include "settings.h"
extern App* apps[];
int numapps;
void switchApp(int ctx);
#include "apps.h"

#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI();
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

void button_loop() {
  btn1.loop();
  btn2.loop();
}

void button_init() {
  btn1.setLongClickHandler([](Button2 & b) {
    Serial.println("Button 1 pressed long.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {
        apps[menu.getCtx()]->onButton1LongClick();
      } else if(orientation == 1) {

      } else if(orientation == 2) {

      } else {
        apps[menu.getCtx()]->onButton1LongClick();
      }
    }
  });
  btn2.setLongClickHandler([](Button2 & b) {
    Serial.println("Button 2 pressed long.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {

      } else if(orientation == 1) {
        apps[menu.getCtx()]->onButton1LongClick();
      } else if(orientation == 2) {
        apps[menu.getCtx()]->onButton1LongClick();
      } else {

      }
    }
  });

  btn1.setClickHandler([](Button2 & b) {
    Serial.println("Button 1 pressed short.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {
        apps[menu.getCtx()]->onButton1Click();
      } else if(orientation == 1) {
        apps[menu.getCtx()]->onButton2Click();
      } else if(orientation == 2) {
        apps[menu.getCtx()]->onButton2Click();
      } else {
        apps[menu.getCtx()]->onButton1Click();
      }
    }
  });
  btn2.setClickHandler([](Button2 & b) {
    Serial.println("Button 2 pressed short.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {
        apps[menu.getCtx()]->onButton2Click();
      } else if(orientation == 1) {
        apps[menu.getCtx()]->onButton1Click();
      } else if(orientation == 2) {
        apps[menu.getCtx()]->onButton1Click();
      } else {
        apps[menu.getCtx()]->onButton2Click();
      }
    }
  });

  btn1.setDoubleClickHandler([](Button2 & b) {
    Serial.println("Button 2 pressed double.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {

      } else if(orientation == 1) {
        apps[menu.getCtx()]->onClose();
        menu.setCtx(0);
        menu.onSetup(tft);
      } else if(orientation == 2) {
        apps[menu.getCtx()]->onClose();
        menu.setCtx(0);
        menu.onSetup(tft);
      } else {

      }
    }
  });
  btn2.setDoubleClickHandler([](Button2 & b) {
    Serial.println("Button 2 pressed double.");
    if (!wakeCallback()) {
      byte orientation = Util::Screen::getRotation();
      if(orientation == 0) {
        apps[menu.getCtx()]->onClose();
        menu.setCtx(0);
        menu.onSetup(tft);
      } else if(orientation == 1) {

      } else if(orientation == 2) {

      } else {
        apps[menu.getCtx()]->onClose();
        menu.setCtx(0);
        menu.onSetup(tft);
      }
    }
  });
}

void switchApp(int ctx) {
  Serial.println("Switching apps");
  apps[menu.getCtx()]->onSetup(tft);
}

WiFiMulti WiFiMulti;

void setup()
{
  Serial.begin(115200);
  Serial.println("Start");
  numapps = (sizeof(apps) / sizeof(App*));
  initialiseConfig();

  byte rotation = configDoc.containsKey("rotation") ? configDoc["rotation"] : 0;

  tft.init();
  Util::Screen::setRotation(tft, rotation);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
    //digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h

    ledcSetup(Util::Screen::pwmLedChannelTFT, Util::Screen::pwmFreq, Util::Screen::pwmResolution);
    ledcAttachPin(TFT_BL, Util::Screen::pwmLedChannelTFT);

    byte brightness = configDoc.containsKey("screenBrightness") ? configDoc["screenBrightness"] : 255;
    Util::Screen::setBrightness(brightness);
  }

  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  WiFi.mode(WIFI_STA);
  JsonArray wifiConfigs = configDoc["wifi"];
  for (JsonObject cfg : wifiConfigs) {
    if (cfg["ssid"]) WiFiMulti.addAP(cfg["ssid"], cfg["password"]);
  }
  initialiseSettings();
  calibrateBattery();

  const char* tz = configDoc.containsKey("timezone") ? configDoc["timezone"].as<char*>() : "GMT0";
  clockFormat = configDoc.containsKey("clockFormat") ? configDoc["clockFormat"].as<char*>() : "%H:%M";
  setenv("TZ", tz, 1);

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_init();
  tzset();

  menu.onSetup(tft);

  Serial.println("Initialising buttons");
  button_init();

  xTaskCreatePinnedToCore(
    loopProg
    ,  "loopProg"
    ,  10000  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL
    ,  0 // Core
  );

  xTaskCreatePinnedToCore(
    loopBackground
    ,  "loopBackground"
    ,  10000  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  NULL
    ,  1 // Core
  );
}

void loopProg(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    button_loop();
    App* app = apps[menu.getCtx()];
    if (!app->getFullscreen()) renderStatus(tft);
    app->render(tft);
    // TODO: Remove this delay without tripping the watchdog
    vTaskDelay(1);
  }
}

bool beganTime = false;
void loopBackground(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    if (wifiEnabled && !hotspotEnabled && WiFiMulti.run() == WL_CONNECTED) {
      int rssi = WiFi.RSSI();
      wifiSignal = wifiStrength(rssi);
    } else {
      wifiSignal = 0;
    }
    for (int i = 0; i < 400; ++i) {
      button_loop();
      if (!apps[menu.getCtx()]->getSleepDisabled()) checkSleep();
      vTaskDelay(10);
    }
  }
}

void loop() {}
