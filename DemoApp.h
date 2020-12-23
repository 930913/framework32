class DemoApp : public App {
  private:
    int c = 0;
    bool shouldRender = true;
  public:
    char* getMenuName() override {
      return "Demo App";
    }

    // Runs once when the app starts. Like the Arduino "setup" function
    void onSetup(TFT_eSPI& tft) override {
      c = 0;
      shouldRender = true;
    };

    // Runs repeatedly for the app's lifecycle. Like the Arduino "loop" function
    void render(TFT_eSPI& tft) override {
      // No need to redraw if not updated - this reduces flicker from repeatedly redrawing
      if (!shouldRender) return;

      // Helper function to blank the screen without overwriting the status bar
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(2);

      tft.setCursor(1, 11, 2); // Note Y offset by 10 to avoid status bar
      tft.println("Demo App");
      tft.setTextSize(1);
      tft.println("Left button adds 1");
      tft.println("Right button subtracts 1");

      tft.setTextSize(2);
      tft.print("Current\ncount: ");
      tft.println(c);

      shouldRender = false;
    };

    void onButton1Click() override {
      ++c;
      shouldRender = true;
    };
    void onButton2Click() override {
      --c;
      shouldRender = true;
    };

    void onClose() override {
      // If you need to clean up when the app closes, here is the place to do it
    }


    bool getFullscreen() override {
      // Return true to hide the status bar
      return false;
    }

    bool getSleepDisabled() override {
      // Return true to prevent sleeping
      return false;
    };

    const unsigned short* getIcon() override {
      return unknownlogo;
    };
};

DemoApp demoapp;
