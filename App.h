#include "unknownlogo.h"
class App {
  public:
    virtual char* getMenuName();

    virtual void onSetup(TFT_eSPI& tft) {};

    virtual void render(TFT_eSPI& tft) {};

    virtual void onClose() {};

    virtual bool getFullscreen() {
      return false;
    };
    virtual bool getSleepDisabled() {
      return false;
    };

    virtual void onButton1Click() {};
    virtual void onButton1LongClick() {};
    virtual void onButton2Click() {};

    virtual const unsigned short* getIcon() {
      return unknownlogo;
    };

  protected:
    JsonObject getSettings() {
      char s[512];
      serializeJson(configDoc, s);
      //Serial.println(s);
      serializeJson(configDoc[this->getMenuName()], s);
      //Serial.println(s);
      return configDoc[this->getMenuName()];
    };
};
