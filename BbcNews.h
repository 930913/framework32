#include "bbcnewslogo.h"
#define numNews 10
class BbcNews : public App {
  private:
    char news[numNews][128];
    char newsDetails[numNews][128];
    int newsI = 0;
    bool rerenderNews = false;
  public:
    char* getMenuName() override {
      return "BBC News";
    }
    const unsigned short* getIcon() override {
      return bbcnewslogo;
    };
    void onSetup(TFT_eSPI tft) override {
      tft.setCursor(1, 10, 2);
      fillScreen(tft, TFT_BLACK);
      tft.setTextSize(1);
      tft.println("Connecting...");
      WiFiClient *client = new WiFiClient;
      HTTPClient http;
      if (http.begin(*client, "http://feeds.bbci.co.uk/news/rss.xml")) {
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

            byte started = 0;
            int startT = 0;
            int startD = 0;
            int endd = 0;
            byte newsNum = 0;
            bool skipItT = true;
            bool skipItD = true;
            while (payload[++endd] && newsNum < numNews) {
              String piece = payload.substring(startT, endd);
              if (piece.endsWith("<title><![CDATA[")) {
                startT = endd;
              }
              if (piece.endsWith("]]></title>")) {
                if (skipItT) {
                  skipItT = false;
                  continue;
                }
                sprintf(news[newsNum], "%.127s",  piece.substring(0, piece.length() - 11).c_str());
                Serial.println(piece);
              }
              piece = payload.substring(startD, endd);
              if (piece.endsWith("<description><![CDATA[")) {
                startD = endd;
              }
              if (piece.endsWith("]]></description>")) {
                if (skipItD) {
                  skipItD = false;
                  continue;
                }
                sprintf(newsDetails[newsNum++], "%.127s", piece.substring(0, piece.length() - 17).c_str());
                Serial.println(piece);
              }
            }
            rerenderNews = true;
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
      delete client;
    }

    void render(TFT_eSPI tft) override {
      if (!rerenderNews) return;
      Serial.print("News: ");
      Serial.println(news[newsI]);
      Serial.println(newsDetails[newsI]);
      fillScreen(tft, TFT_BLACK);
      tft.setCursor(1, 10, 2);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.println(news[newsI]);
      tft.println("-----------------------");
      tft.println(newsDetails[newsI]);
      rerenderNews = false;
    }

    void newsUp() {
      newsI = max(0, newsI - 1);
      rerenderNews = true;
    }
    void newsDown() {
      newsI = min(numNews, newsI + 1);
      rerenderNews = true;
    }
    void onButton1Click() override {
      newsDown();
    };
    void onButton2Click() override {
      newsUp();
    };

    void onClose() override {
      for (int i = 0; i < numNews; ++i) if (news[i] != NULL) delete news[i];
    }
};

BbcNews bbcnews;
