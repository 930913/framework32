class About : public App {
  public:
    char* getMenuName() override {
      return "About";
    }

    void onSetup(TFT_eSPI tft) override {
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);

      tft.setCursor(0, 10, 2);

      tft.println("About");
      tft.println("This multi-application system was made using Framework32");
    };
};

About about;
