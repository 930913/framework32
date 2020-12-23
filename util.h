namespace Util {
  namespace Screen {
    namespace {
      const int pwmFreq = 5000;
      const int pwmResolution = 8;
      const int pwmLedChannelTFT = 0;

      byte orientation = 0;
      byte brightness = 255;

      bool screenOn = true;
    }

    void setRotation(TFT_eSPI& tft, uint8_t rotation) {
      orientation = rotation;
      tft.setRotation(rotation);
    }
    byte getRotation() {
      return orientation;
    }

    byte getHeight() {
      return orientation % 2 ? TFT_WIDTH : TFT_HEIGHT;
    }

    byte getWidth() {
      return orientation % 2 ? TFT_HEIGHT: TFT_WIDTH;
    }

    void setBrightness(byte b) {
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

    void fillScreen(TFT_eSPI& tft, uint32_t colour) {
      tft.fillRect(0, 10, getWidth(), getHeight() - 10, colour);
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
