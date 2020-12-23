class Menu : public App {
  private:
    int menuSelect = 1;
    int renderedMenu = -1;
    int ctx = 0;
    int yOffset = 0;
    static const int tileh = 75 + 1;
    static const int tilew = 64 + 1;
    int xIcons;
    int yIcons;
    int maxDisplayed;
  public:
    char* getMenuName() override {
      return "Menu";
    }
    void onSetup(TFT_eSPI& tft) override {
      menuSelect = 1;
      renderedMenu = -1;
      yOffset = 0;
      xIcons = Util::Screen::getWidth() / tilew;
      yIcons = (Util::Screen::getHeight() - 10) / tileh;
      maxDisplayed = xIcons * yIcons;
    };
    void render(TFT_eSPI& tft) override {
      //Serial.println("Rendering menu");
      if (menuSelect == renderedMenu) return;
      //Serial.println("Menu is being rendered");
      int xSpacing = (Util::Screen::getWidth() - (xIcons * tilew)) / (xIcons + 1);
      int ySpacing = ((Util::Screen::getHeight() - 10) - (yIcons * tileh)) / (yIcons + 1);
      //Serial.printf("xIcons: %d, yIcons: %d, maxDisplayed: %d, xSpacing: %d, ySpacing: %d\n", xIcons, yIcons, maxDisplayed, xSpacing, ySpacing);

      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);
      //Serial.println(numapps);
      //Serial.println(menuSelect);

      for (int i = 1 + yOffset * xIcons; i < numapps && i <= maxDisplayed + yOffset * xIcons; ++i) {
        byte menuX = (i - 1) % xIcons;
        byte menuY = (i - 1) / xIcons - yOffset;
        int x = xSpacing + (xSpacing + tilew) * menuX;
        int y = 11 + ySpacing + menuY * (ySpacing + tileh);
        //Serial.printf("menuX: %d, menuY: %d, x: %d, y: %d\n", menuX, menuY, x, y);
        tft.pushImage(x + 1, y + 1, 64, 64, apps[i]->getIcon());
        tft.drawCentreString(apps[i]->getMenuName(), x + 33, y + 66, 1);
        if (menuSelect == i) tft.drawRect(x, y, tilew, tileh, TFT_WHITE);
      }
      renderedMenu = menuSelect;
    };
    void menuUp() {
      Serial.println("Menu up");
      menuSelect = max(1, menuSelect - 1);
      if(menuSelect - 1 < yOffset * xIcons) --yOffset;
    };
    void menuDown() {
      Serial.println("Menu down");
      menuSelect = min((int)numapps - 1, menuSelect + 1);
      if(menuSelect > maxDisplayed + yOffset * xIcons) ++yOffset;
    };
    void onButton1Click() override {
      menuDown();
    };
    void onButton2Click() override {
      menuUp();
    };
    void onButton1LongClick() override {
      ctx = menuSelect;
      switchApp(ctx);
    };
    ;
    void setCtx(int context) {
      ctx = context;
    }
    int getCtx() {
      return ctx;
    };
};

Menu menu;
