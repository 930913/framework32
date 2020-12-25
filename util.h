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

    // TODO: Probably need to make this configurable, as there may be devices that are the opposite
    bool isPortrait() {
      return orientation == 0 || orientation == 2;
    }
    bool isLandscape() {
      return orientation == 1 || orientation == 3;
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

    namespace Grid {
      // x/y = orientation
      // 1/2/3/4 = how many blocks of the screen make up this grid
      // l/t / m / r/b = coordinate for left/top / middle / right/bottom
      // 1 .. blocks = which number block in grid
      // E.g. y3m2() will give you the y coordinate for the middle of the second block in a grid of three
      // E.g. x2r1() will give you the y coordinate for the right of the first block in a grid of two

      int calculate(int len, int pieces, char alignment, int pieceNum) {
        int block = (len / pieces);
        if(alignment == 'l' || alignment == 't') return block * (pieceNum - 1);
        if(alignment == 'm') return block * (pieceNum - 1) + block / 2;
        if(alignment == 'r' || alignment == 'b') return block * pieceNum;
      }

      int x1l1() { return calculate(getWidth(), 1, 'l', 1); }
      int x1m1() { return calculate(getWidth(), 1, 'm', 1); }
      int x1r1() { return calculate(getWidth(), 1, 'r', 1); }

      int x2l1() { return calculate(getWidth(), 2, 'l', 1); }
      int x2m1() { return calculate(getWidth(), 2, 'm', 1); }
      int x2r1() { return calculate(getWidth(), 2, 'r', 1); }
      int x2l2() { return calculate(getWidth(), 2, 'l', 2); }
      int x2m2() { return calculate(getWidth(), 2, 'm', 2); }
      int x2r2() { return calculate(getWidth(), 2, 'r', 2); }

      int x3l1() { return calculate(getWidth(), 3, 'l', 1); }
      int x3m1() { return calculate(getWidth(), 3, 'm', 1); }
      int x3r1() { return calculate(getWidth(), 3, 'r', 1); }
      int x3l2() { return calculate(getWidth(), 3, 'l', 2); }
      int x3m2() { return calculate(getWidth(), 3, 'm', 2); }
      int x3r2() { return calculate(getWidth(), 3, 'r', 2); }
      int x3l3() { return calculate(getWidth(), 3, 'l', 3); }
      int x3m3() { return calculate(getWidth(), 3, 'm', 3); }
      int x3r3() { return calculate(getWidth(), 3, 'r', 3); }

      int x4l1() { return calculate(getWidth(), 4, 'l', 1); }
      int x4m1() { return calculate(getWidth(), 4, 'm', 1); }
      int x4r1() { return calculate(getWidth(), 4, 'r', 1); }
      int x4l2() { return calculate(getWidth(), 4, 'l', 2); }
      int x4m2() { return calculate(getWidth(), 4, 'm', 2); }
      int x4r2() { return calculate(getWidth(), 4, 'r', 2); }
      int x4l3() { return calculate(getWidth(), 4, 'l', 3); }
      int x4m3() { return calculate(getWidth(), 4, 'm', 3); }
      int x4r3() { return calculate(getWidth(), 4, 'r', 3); }
      int x4l4() { return calculate(getWidth(), 4, 'l', 4); }
      int x4m4() { return calculate(getWidth(), 4, 'm', 4); }
      int x4r4() { return calculate(getWidth(), 4, 'r', 4); }

      int y1t1() { return calculate(getHeight() - 10, 1, 't', 1) + 10; }
      int y1m1() { return calculate(getHeight() - 10, 1, 'm', 1) + 10; }
      int y1b1() { return calculate(getHeight() - 10, 1, 'b', 1) + 10; }

      int y2t1() { return calculate(getHeight() - 10, 2, 't', 1) + 10; }
      int y2m1() { return calculate(getHeight() - 10, 2, 'm', 1) + 10; }
      int y2b1() { return calculate(getHeight() - 10, 2, 'b', 1) + 10; }
      int y2t2() { return calculate(getHeight() - 10, 2, 't', 2) + 10; }
      int y2m2() { return calculate(getHeight() - 10, 2, 'm', 2) + 10; }
      int y2b2() { return calculate(getHeight() - 10, 2, 'b', 2) + 10; }

      int y3t1() { return calculate(getHeight() - 10, 3, 't', 1) + 10; }
      int y3m1() { return calculate(getHeight() - 10, 3, 'm', 1) + 10; }
      int y3b1() { return calculate(getHeight() - 10, 3, 'b', 1) + 10; }
      int y3t2() { return calculate(getHeight() - 10, 3, 't', 2) + 10; }
      int y3m2() { return calculate(getHeight() - 10, 3, 'm', 2) + 10; }
      int y3b2() { return calculate(getHeight() - 10, 3, 'b', 2) + 10; }
      int y3t3() { return calculate(getHeight() - 10, 3, 't', 3) + 10; }
      int y3m3() { return calculate(getHeight() - 10, 3, 'm', 3) + 10; }
      int y3b3() { return calculate(getHeight() - 10, 3, 'b', 3) + 10; }

      int y4t1() { return calculate(getHeight() - 10, 4, 't', 1) + 10; }
      int y4m1() { return calculate(getHeight() - 10, 4, 'm', 1) + 10; }
      int y4b1() { return calculate(getHeight() - 10, 4, 'b', 1) + 10; }
      int y4t2() { return calculate(getHeight() - 10, 4, 't', 2) + 10; }
      int y4m2() { return calculate(getHeight() - 10, 4, 'm', 2) + 10; }
      int y4b2() { return calculate(getHeight() - 10, 4, 'b', 2) + 10; }
      int y4t3() { return calculate(getHeight() - 10, 4, 't', 3) + 10; }
      int y4m3() { return calculate(getHeight() - 10, 4, 'm', 3) + 10; }
      int y4b3() { return calculate(getHeight() - 10, 4, 'b', 3) + 10; }
      int y4t4() { return calculate(getHeight() - 10, 4, 't', 4) + 10; }
      int y4m4() { return calculate(getHeight() - 10, 4, 'm', 4) + 10; }
      int y4b4() { return calculate(getHeight() - 10, 4, 'b', 4) + 10; }
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
