#include "certs.h"
#include "hivelogo.h"

class Hive : public App {
  public:
    char* getMenuName() override {
      return "Hive";
    }
    const unsigned short* getIcon() override {
      return hivelogo;
    };
    void onSetup(TFT_eSPI tft) override {
      JsonObject hiveSettings = getSettings();
      String username = hiveSettings["username"];
      Serial.print("Username for Hive: "); Serial.println(username);
      String password = hiveSettings["password"];
      WiFiClientSecure *client = new WiFiClientSecure;
      if (client) {
        client -> setCACert(amazonRootCert);

        {
          // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
          HTTPClient https;
          fillScreen(tft, TFT_BLACK);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);

          tft.setCursor(0, 10, 2);
          tft.println("Connecting...");

          Serial.print("[HTTPS] begin...\n");
          if (https.begin(*client, "https://beekeeper.hivehome.com/1.0/cognito/login")) {  // HTTPS
            https.addHeader("Content-Type", "application/json");
            Serial.print("[HTTPS] GET...\n");
            // start connection and send HTTP header
            String post = String("{\"username\":\"") + username + "\",\"password\":\"" + password + "\",\"devices\":false,\"products\":true,\"actions\":false,\"homes\":false}";
            int httpCode = https.POST(post);

            delay(1);
            // httpCode will be negative on error
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = https.getString();
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

                JsonArray products = doc["products"];
                const float target = products[0]["state"]["target"];
                const float temperature = products[0]["props"]["temperature"];
                const char* hotwater = products[1]["state"]["status"];
                tft.print("Temperature: ");
                tft.println(temperature);
                tft.print("Target: ");
                tft.println(target);
                tft.print("Hot water: ");
                tft.println(hotwater);
              } else {
                Serial.printf("[HTTPS] failed with status code: %d\n", httpCode);
                tft.printf("Failed with status code: %d\n", httpCode);
              }
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              tft.printf("Failed with error: %s\n", https.errorToString(httpCode).c_str());
            }

            https.end();
          } else {
            Serial.printf("[HTTPS] Unable to connect\n");
            tft.printf("Failed to connect");
          }

          // End extra scoping block
        }

        delete client;
      } else {
        tft.println("Unable to create client");
      }
    }
};

Hive hive;
