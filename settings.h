#include "wifisymbols.h"
#include "batterycharge.h"
#include <NTPClient.h>
#define Threshold 40
#define ADC_PIN 34

long timeToSleep = 10000;
bool wifiEnabled = true;
bool hotspotEnabled = false;
byte wifiSignal = 0;
byte wifiStrength(int rssi) {
  //  Serial.print("RSSI: ");
  //  Serial.println(rssi);
  if (rssi > -60) return 4;
  if (rssi > -70) return 3;
  if (rssi > -80) return 2;
  if (rssi == 0) return 0;
  return 1;
}

unsigned long lastStatusRender = 0;

void initialiseSettings() {
  timeToSleep = configDoc["sleepTimeout"];
}

int vref = 1100;
void calibrateBattery() {
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
  //Check type of calibration value used to characterize ADC
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
    Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
  } else {
    Serial.println("Default Vref: 1100mV");
  }
}
float getBatteryVoltage() {
  uint16_t v = analogRead(ADC_PIN);
  float batteryVoltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
  return batteryVoltage;
}
bool drawBatterySymbol(TFT_eSPI tft, float voltage, int x) {
  if (voltage > 4.5) {
    tft.pushImage(x, 0,  8, 8, batteryCharge);
    return true;
  }
  if (voltage > 3.9) {
    tft.pushImage(x, 0,  8, 8, batteryHigh);
    return true;
  }
  if (voltage > 3.5) {
    tft.pushImage(x, 0,  8, 8, batteryMid);
    return true;
  }
  if (voltage > 2.7) {
    tft.pushImage(x, 0,  8, 8, batteryLow);
    return true;
  }
  // TODO: Shut down to protect the battery!
}

bool drawWifiSymbol(TFT_eSPI tft, byte strength, int x) {
  //  Serial.print("Signal strength drawing: ");
  //  Serial.println(strength);
  if (hotspotEnabled) {
    tft.pushImage(x, 0,  8, 8, wifiHotspot);
    return true;
  }
  if (strength == 1) {
    tft.pushImage(x, 0,  8, 8, wifi1);
    return true;
  }
  if (strength == 2) {
    tft.pushImage(x, 0,  8, 8, wifi2);
    return true;
  }
  if (strength == 3) {
    tft.pushImage(x, 0,  8, 8, wifi3);
    return true;
  }
  if (strength == 4) {
    tft.pushImage(x, 0,  8, 8, wifi4);
    return true;
  }
  return false;
}

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void drawClock(TFT_eSPI tft) {
  //  Serial.println("Rendering clock");
  if (timeClient.getEpochTime() > 1000000000) {
    tft.printf("%02d", timeClient.getHours());
    tft.print(":");
    tft.printf("%02d", timeClient.getMinutes());
    //Serial.print(timeClient.getEpochTime());
  } else {
    tft.print("--:--");
  }
}

void renderStatus(TFT_eSPI tft) {
  //  Serial.println("Rendering status");
  if (lastStatusRender > millis() - 5000) return;

  tft.fillRect(0, 0, TFT_WIDTH, 10, 0x0000);
  tft.setCursor(0, 0);
  float voltage = getBatteryVoltage();
  drawBatterySymbol(tft, voltage, TFT_WIDTH - 10);
  bool wifiDrawn = drawWifiSymbol(tft, wifiSignal, TFT_WIDTH - 20);
  // if (wifiDrawn) offset next icon
  tft.drawLine(0, 9, TFT_WIDTH, 9, 0x1111);

  drawClock(tft);

  lastStatusRender = millis();
}

long sleepTimer = 0;
bool sleeping = false;
void resetSleepTimer() { sleepTimer = millis(); }
bool wakeCallback() {
  resetSleepTimer();
  if (!sleeping) return false;
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  sleeping = false;
  return true;
}
void checkSleep() {
  if (!sleeping && millis() - sleepTimer > timeToSleep) {
    // TODO: Perhaps use a more efficient sleep, instead of just turning the backlight off?
    // We are also still processing the app as normal.
    digitalWrite(TFT_BL, !TFT_BACKLIGHT_ON);
    sleeping = true;
  }
};
