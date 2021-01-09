#include "marlinicons.h"
#include "octoprintlogo.h"

class Octoprint : public App {
  private:
    bool shouldRender;
    long lastUpdated;
    DynamicJsonDocument printer = DynamicJsonDocument(2000);
    DynamicJsonDocument job = DynamicJsonDocument(2000);
    int statusCode;
    uint16_t hotmap[16] = {0b0000000000000000, 0b0001000000000000, 0b0010000000000000, 0b0011000000000000, 0b0100000000000000, 0b0100000000000000, 0b0101000000000000, 0b0110000000000000, 0b0000000000000000, 0b1000000000000000, 0b1001000000000000, 0b1010000000000000, 0b1011000000000000, 0b1100000000000000, 0b1110000000000000, 0b1111000000000000};
    uint16_t coldmap[16] = {0b0000000000000000, 0b0000000000000010, 0b0000000000000100, 0b0000000000000110, 0b0000000000001000, 0b0000000000001000, 0b0000000000001010, 0b0000000000001100, 0b0000000000000000, 0b0000000000010000, 0b0000000000010010, 0b0000000000010100, 0b0000000000010110, 0b0000000000011000, 0b0000000000011100, 0b0000000000011110};
    bool sleepDisabled;
    int refreshTime;
    String server;
  public:
    char* getMenuName() override {
      return "Octoprint";
    }

    void onSetup(TFT_eSPI& tft) override {
      sleepDisabled = getSettings().containsKey("sleepDisabled") ? getSettings()["sleepDisabled"] : false;
      refreshTime = getSettings().containsKey("refreshTime") ? getSettings()["refreshTime"] : 5000;
      server = getSettings()["server"].as<String>();
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);
      tft.drawCentreString("Connecting...", Util::Screen::Grid::x1m1(), Util::Screen::Grid::y1m1(), 2);
      update(tft);
    }
    void render(TFT_eSPI& tft) override {
      if (lastUpdated + refreshTime < millis()) {
        update(tft);
      }
      if (!shouldRender) return;

      if (statusCode == HTTP_CODE_OK) {
        char tmp[6];
        int x = Util::Screen::Grid::x1m1();
        int y = Util::Screen::Grid::y3t1();
        String state = printer["state"]["text"];
        tft.setTextColor(TFT_WHITE);
        JsonObject temperatures = printer["temperature"];
        Util::Screen::fillScreen(tft, TFT_BLACK);
        tft.setCursor(0, 10, 2);
        tft.drawCentreString("State:", x, y, 2);
        tft.drawCentreString(state, x, y + 15, 2);

        int i = 0;
        for (JsonPair temp : temperatures) {
          const char* toolName = temp.key().c_str();
          JsonObject t = temp.value().as<JsonObject>();
          float actual = t["actual"];
          float target = t["target"];
          bool isBed = toolName[0] == 'b';
          uint8_t* icon = isBed ? ((actual < target) ? bed_heated_64x64x4 : bed_64x64x4) : ((actual < target) ? hotend_heated_64x64x4 : hotend_64x64x4);
          uint16_t* iconColourmap = (actual > 40) ? hotmap : coldmap;

          int y = Util::Screen::Grid::y3m2();
          int x = i++ * 64;
          tft.pushImage(x, y - 32, 64, 64, icon, false, iconColourmap);
          sprintf(tmp, "%.2f C", target);
          tft.drawCentreString(tmp, x + 32, y - 37, 2);
          sprintf(tmp, "%.2f C", actual);
          tft.drawCentreString(tmp, x + 32, y + 20, 2);
        }

        String fname = job["job"]["file"]["name"].isNull() ? "No file loaded" : job["job"]["file"]["name"].as<String>();
        int pt = job["progress"]["printTime"];
        int ptl = job["progress"]["printTimeLeft"];
        int ptt = pt + ptl;
        int pth = pt / 3600;
        int ptm = (pt / 60) % 60;
        int pts = pt % 60;
        int ptth = ptt / 3600;
        int pttm = (ptt / 60) % 60;
        int ptts = ptt % 60;
        float completion = job["progress"]["completion"];

        tft.setCursor(0, y + 30, 2);
        tft.println(fname);
        y = Util::Screen::Grid::y3b3();
        if (ptth) {
          if (pth) {
            sprintf(tmp, "%dh%dm%ds / %dh%dm", pth, ptm, pts, ptth, pttm);
          } else {
            sprintf(tmp, "%dm%ds / %dh%dm", ptm, pts, ptth, pttm);
          }
        } else {
          sprintf(tmp, "%dm%ds / %dm", ptm, pts, pttm);
        }
        tft.drawCentreString(tmp, x, y - 30, 2);

        int progressBarWidth = (int)(Util::Screen::getWidth() * completion / 100);
        char perc[7];
        sprintf(perc, "%.1f%%", completion);
        Serial.println(perc);
        tft.fillRect(0, y - 15, progressBarWidth, 15, TFT_BLUE);
        tft.setTextColor(TFT_GREY);
        tft.drawCentreString(String(perc), x, y - 15, 2);

      } else if (statusCode == HTTP_CODE_CONFLICT) {
        Util::Screen::fillScreen(tft, TFT_BLACK);
        tft.setCursor(0, 10, 4);
        tft.println("Printer is not operational");
      } else {
        Serial.printf("[HTTPS] failed with status code: %d\n", statusCode);
        tft.printf("Failed with status code: %d\n", statusCode);
      }

      shouldRender = false;
    }

    bool getFullscreen() override {
      return false;
    }

    bool getSleepDisabled() override {
      return sleepDisabled;
    };

    const unsigned short* getIcon() override {
      return octoprintlogo;
    };

    void update(TFT_eSPI& tft) {
      tft.setCursor(0, 10, 1);
      fetch(server + "/api/printer", printer, tft);
      if (statusCode == HTTP_CODE_OK) {
        fetch(server + "/api/job", job, tft);
      }
      shouldRender = true;
      lastUpdated = millis();
    }

    void fetch(String url, JsonDocument& jd, TFT_eSPI& tft) {
      WiFiClientSecure *client = new WiFiClientSecure;
      if (client) {
        {
          HTTPClient https;

          Serial.print("[HTTPS] begin...\n");
          if (https.begin(*client, url)) {
            https.addHeader("X-Api-Key", getSettings()["apikey"].as<String>());
            Serial.print("[HTTPS] GET...\n");
            int httpCode = https.GET();
            delay(1);
            statusCode = httpCode;
            if (httpCode > 0) {
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = https.getString();
                Serial.println(payload);
                Serial.println(sizeof(payload));

                DeserializationError err = deserializeJson(jd, payload);
                if (err) {
                  tft.print("deserialization error ");
                  tft.println(err.c_str());
                }
              }
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              tft.printf("Failed with error: %s\n", https.errorToString(httpCode).c_str());
            }
            https.end();
          } else {
            Serial.printf("[HTTP] Unable to connect\n");
            tft.printf("Failed to connect");
          }
        }
        delete client;
      } else {
        tft.println("Unable to create client");
      }
    }
};

Octoprint octoprint;
