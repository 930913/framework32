#include <TOTP.h>
#include "otplogo.h"

class Otp : public App {
  private:
    bool shouldRender = true;
    long lastUpdated;
    TOTP* totp;
    int timeStep = 30;
    byte* hmacKey = NULL;
    int keyLen;
  public:
    char* getMenuName() override {
      return "OTP Auth";
    }

    void onSetup(TFT_eSPI& tft) override {
      String s = getSettings()["base32Key"];
      byte buf[s.length() + 1];
      s.getBytes(buf, s.length() + 1);
      keyLen = Util::Base32::fromBase32(buf, s.length(), (byte*&)hmacKey);
      totp = new TOTP(hmacKey, keyLen, 30);
      shouldRender = true;
    };

    void render(TFT_eSPI& tft) override {
      if (lastUpdated + 1000 > millis() && !shouldRender) return;

      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);

      time_t now;
      time(&now);
      String code = String(totp->getCode(now));

      int timeLeft = timeStep - now % timeStep;
      float timeLeftRatio = timeLeft / (float)timeStep;

      if (timeLeft <= 5) tft.setTextColor(TFT_RED, TFT_BLACK);

      tft.drawCentreString(code, Util::Screen::Grid::x1m1(), Util::Screen::Grid::y1m1(), 4);
      int barWidth = Util::Screen::getWidth() * timeLeftRatio;
      int barOffset = (Util::Screen::getWidth() - barWidth) / 2;
      tft.fillRect(barOffset, Util::Screen::Grid::y1m1() + 30, barWidth, 5, timeLeft <= 5 ? TFT_RED : TFT_WHITE);

      lastUpdated = millis();
      shouldRender = false;
    };

    void onClose() override {
      free(totp);
      free(hmacKey);
    }

    bool getFullscreen() override {
      return false;
    }

    bool getSleepDisabled() override {
      return true;
    };

    const unsigned short* getIcon() override {
      return OtpLogo;
    };
};

Otp otp;
