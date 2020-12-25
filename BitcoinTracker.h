#include "bitcoinlogo.h"

class BitcoinTracker : public App {
  private:
    long lastUpdated;
    void drawLogo(TFT_eSPI& tft) {
      Util::Screen::fillScreen(tft, TFT_BLACK);
      int x,y;
      if(Util::Screen::isLandscape()) {
        x = Util::Screen::Grid::x2m1() - 32;
        y = Util::Screen::Grid::y1m1() - 32;
      } else {
        x = Util::Screen::Grid::x1m1() - 32;
        y = Util::Screen::Grid::y2m1() - 32;
      }
      tft.pushImage(x, y, 64, 64, bitcoinlogo);
    }
  public:
    char* getMenuName() override {
      return "BTC Trackr";
    }
    const unsigned short* getIcon() override {
      return bitcoinlogo;
    };
    void onSetup(TFT_eSPI& tft) override {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
      drawLogo(tft);
      tft.drawCentreString("Connecting...", Util::Screen::Grid::x1m1(), Util::Screen::Grid::y1m1(), 2);
      update(tft);
    }
    void render(TFT_eSPI& tft) override {
      if (lastUpdated + 60000 < millis()) {
        update(tft);
      }
    }
    void update(TFT_eSPI& tft) {
      WiFiClient *client = new WiFiClient;
      if (client) {

        {
          // Add a scoping block for HTTPClient http to make sure it is destroyed before WiFiClient *client is
          HTTPClient http;

          Serial.print("[HTTP] begin...\n");
          if (http.begin(*client, "http://api.coindesk.com/v2/bpi/currentprice.json")) {
            http.addHeader("Content-Type", "application/json");
            Serial.print("[HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();

            delay(1);
            // httpCode will be negative on error
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTP] GET... code: %d\n", httpCode);

              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                Serial.println(payload);
                Serial.println(sizeof(payload));

                delay(1);
                DynamicJsonDocument doc(20000);
                delay(1);
                DeserializationError err = deserializeJson(doc, payload);
                if (err) {
                  tft.print("deserialization error ");
                  tft.println(err.c_str());
                }

                JsonObject rates = doc["bpi"];

                Util::Screen::fillScreen(tft, TFT_BLACK);
                drawLogo(tft);

                int i = 0;
                for(JsonPair rate : rates) {
                  int x,y;
                  if(Util::Screen::isLandscape()) {
                    x = Util::Screen::Grid::x2m2() - 60;
                    y = Util::Screen::Grid::y1m1() - 45 + 15 * i++;
                  } else {
                    x = Util::Screen::Grid::x1m1() - 60;
                    y = Util::Screen::Grid::y2m2() - 45 + 15 * i++;
                  }
                  Serial.printf("x: %d, y: %d\n", x, y);
                  tft.setCursor(x, y, 2);
                  tft.print(rate.value()["rate"].as<char*>());
                  tft.print(" ");
                  tft.println(rate.value()["code"].as<char*>());
                }
                tft.drawCentreString("Updated @ ", Util::Screen::isLandscape() ? Util::Screen::Grid::x2m2() : Util::Screen::Grid::x1m1(), Util::Screen::getHeight() - 45, 2);
                const char* updated = doc["time"]["updated"].as<char*>();
                char d[13];
                sprintf(d, "%.12s", updated);
                const char* t = updated + 13;
                if(Util::Screen::isLandscape()) {
                  tft.drawCentreString(d, Util::Screen::Grid::x2m2(), Util::Screen::getHeight() - 30, 2);
                  tft.drawCentreString(t, Util::Screen::Grid::x2m2(), Util::Screen::getHeight() - 15, 2);
                } else {
                  tft.drawCentreString(d, Util::Screen::Grid::x1m1(), Util::Screen::getHeight() - 30, 2);
                  tft.drawCentreString(t, Util::Screen::Grid::x1m1(), Util::Screen::getHeight() - 15, 2);
                }
              } else {
                Serial.printf("[HTTP] failed with status code: %d\n", httpCode);
                tft.printf("Failed with status code: %d\n", httpCode);
              }
            } else {
              Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
              tft.printf("Failed with error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
          } else {
            Serial.printf("[HTTP] Unable to connect\n");
            tft.printf("Failed to connect");
          }

          // End extra scoping block
        }
        delete client;
      } else {
        tft.println("Unable to create client");
      }
      lastUpdated = millis();
    }
};

BitcoinTracker bitcointracker;
