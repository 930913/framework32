#include "wifiscanlogo.h"
class WifiScan : public App {
  private:
    bool wifiOpen = false;
  public:
    char* getMenuName() override {
      return "Scan WiFi";
    }
    void onSetup(TFT_eSPI& tft) override {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextDatum(MC_DATUM);
      tft.setTextSize(1);

      tft.drawString("Scan Network", tft.width() / 2, tft.height() / 2);

      WiFi.mode(WIFI_STA);
      wifiOpen = true;
    }

    void render(TFT_eSPI& tft) override {
      char buff[512];
      int16_t n = WiFi.scanNetworks();
      if (!wifiOpen) return;
      Util::Screen::fillScreen(tft, TFT_BLACK);
      if (n == 0) {
        tft.drawString("No networks found", tft.width() / 2, tft.height() / 2);
      } else {
        tft.setTextDatum(TL_DATUM);
        tft.setCursor(0, 10);
        for (int i = 0; i < n; ++i) {
          sprintf(buff,
                  "[%d]:%s(%d)",
                  i + 1,
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i));
          tft.println(buff);
        }
      }
    }

    void onClose() override {
      wifiOpen = false;
    }

    bool getSleepDisabled() override {
      return true;
    };

    const unsigned short* getIcon() override {
      return wifiscanlogo;
    };
};

WifiScan wifiscan;
