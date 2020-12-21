class Menu : public App {
  private:
    int menuSelect = 1;
    int renderedMenu = -1;
    int ctx = 0;
    int yOffset = 0;
    byte maxDisplayed = 6;
  public:
    char* getMenuName() override {
      return "Menu";
    }
    void onSetup(TFT_eSPI tft) override {
      menuSelect = 1;
      renderedMenu = -1;
      yOffset = 0;
    };
    void render(TFT_eSPI tft) override {
      //Serial.println("Rendering menu");
      if (menuSelect == renderedMenu) return;
      //Serial.println("Menu is being rendered");
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);
      //Serial.println(numapps);
      //Serial.println(menuSelect);

      for (int i = 1 + yOffset * 2; i < numapps && i <= maxDisplayed + yOffset * 2; i++) {
        bool lhs = i % 2 == 1;
        Serial.print("Menu yOffset: ");
        byte menuY = i / 2 - !lhs - yOffset;
        //Serial.println(yOffset);
        //Serial.println(menuSelect);
        tft.pushImage(lhs ? 2 : 69, 11 + menuY * 77, 64, 64, apps[i]->getIcon());
        tft.setCursor(lhs ? 3 : 70, 10 + 66 + menuY * 77, 1);
        tft.println(apps[i]->getMenuName());
        if (menuSelect == i) tft.drawRect(lhs ? 1 : 68, 10 + menuY * 77, 66, 76, TFT_WHITE);
      }
      renderedMenu = menuSelect;
    };
    void menuUp() {
      Serial.println("Menu up");
      menuSelect = max(1, menuSelect - 1);
      if(menuSelect - 1 < yOffset * 2) --yOffset;
    };
    void menuDown() {
      Serial.println("Menu down");
      menuSelect = min((int)numapps - 1, menuSelect + 1);
      if(menuSelect > maxDisplayed + yOffset * 2) ++yOffset;
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
