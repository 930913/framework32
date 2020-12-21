namespace Util {
  namespace Screen {
    const int pwmFreq = 5000;
    const int pwmResolution = 8;
    const int pwmLedChannelTFT = 0;

    byte orientation = 0;
    byte brightness = 255;

    bool screenOn = true;

    void fillScreen(TFT_eSPI tft, uint32_t colour) {
      tft.fillRect(0, 10, TFT_WIDTH, TFT_HEIGHT - 10, colour);
    }

    void setRotation(TFT_eSPI tft, byte rotation) {
      orientation = rotation;
      tft.setRotation(rotation);
    }
    void setBrightness(TFT_eSPI tft, byte b) {
      brightness = b;
      ledcWrite(pwmLedChannelTFT, b);
    }

    void setScreenOn() {
      screenOn = true;
      ledcWrite(pwmLedChannelTFT, brightness);
    }
    void setScreenOff() {
      screenOn = false;
      ledcWrite(pwmLedChannelTFT, 0);
    }

  }

  //! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
  void espDelay(int ms)
  {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
  }
}
