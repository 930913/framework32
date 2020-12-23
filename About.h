class About : public App {
  private:
    static uint8_t conv2d(const char* p) {
      uint8_t v = 0;
      if ('0' <= *p && *p <= '9')
        v = *p - '0';
      return 10 * v + *++p - '0';
    }
    uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time
  public:
    char* getMenuName() override {
      return "About";
    }

    void onSetup(TFT_eSPI& tft) override {
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);

      tft.setCursor(0, 10, 2);

      tft.println("About");
      tft.println("This multi-application system was made using Framework32");
      tft.printf("Compiled:\n - On: %s\n - At: %02d:%02d:%02d", __DATE__, hh, mm, ss);
    };
};

About about;
