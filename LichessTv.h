#include <ArduinoWebsockets.h>
#include "chesspieces.h"
#include "lichesslogo.h"
using namespace websockets;

class LichessTv : public App {
  private:
    WebsocketsClient wc;
    bool shouldRender;
    bool eventsRegistered = false;
    char board[64];
    char fen[100] = "rnbqkbnr/ppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    byte lastMoveFrom;
    byte lastMoveTo;
    String wnick;
    String bnick;
    String wtitle;
    String btitle;
    int wrating;
    int brating;
    float wclock;
    float bclock;
    long lastUpdate;
    enum GameState { LOADING, GAME_ON, WHITE_WIN, BLACK_WIN, DRAW, DISCONNECTED, RELOAD };
    GameState gameState;
    int ply;
    bool isCheck;

    static const unsigned long lightSquareColour = 0xE695;
    static const unsigned long darkSquareColour = 0xB44C;
    static const unsigned long movedLightSquareColour = 0xCE8D;
    static const unsigned long movedDarkSquareColour = 0xAD07;
    static const int boardOffsetX = 3;
    static const int boardOffsetY = 45;

    static const unsigned short* getPiece(char piece) {
      switch (piece) {
        case 'P': return pawnw;
        case 'p': return pawnb;
        case 'R': return rookw;
        case 'r': return rookb;
        case 'N': return knightw;
        case 'n': return knightb;
        case 'B': return bishopw;
        case 'b': return bishopb;
        case 'K': return kingw;
        case 'k': return kingb;
        case 'Q': return queenw;
        case 'q': return queenb;
      }
    }

    void clearBoard() {
      //Serial.println("Clearing board");
      for (int i = 0; i <= 64; ++i) board[i] = 0;
    }

    void parseFen() {
      //Serial.println("Parsing FEN");
      //Serial.println(fen);
      char* b = board;
      char* ptr = fen;
      while (*ptr) {
        if (*ptr > 60) {
          *b++ = *ptr++;
        } else if (*ptr == '/') {
          ++ptr; // Ignore? Assumes full FEN
        } else {
          b += *ptr++ - '0';
        }
      }
    }

    void drawBoard(TFT_eSPI tft) {
      //Serial.println("Drawing board");
      for (int i = 0; i < 64; ++i) {
        byte file = i % 8;
        byte rank = i / 8;
        int x = file * 16 + boardOffsetX;
        int y = (rank * 16) + boardOffsetY;
        bool isLight = (rank + (file % 2)) % 2;
        bool check = isCheck && ((board[i] == 'K' && ply % 2 == 0) || (board[i] == 'k' && ply % 2 == 1));
        long colour = check ? TFT_RED : (isLight ? (
                                           (i == lastMoveTo || i == lastMoveFrom) ? movedLightSquareColour : lightSquareColour
                                         ) : (
                                           (i == lastMoveTo || i == lastMoveFrom) ? movedDarkSquareColour : darkSquareColour
                                         ));
        tft.fillRect(x, y, 16, 16, colour);
        //Serial.print(board[i]);
        if (board[i]) tft.pushImage(x, y, 16, 16, getPiece(board[i]), 0xF800);
      }
    }

    void drawUsers(TFT_eSPI tft) {
      //Serial.println("Drawing users");
      tft.setCursor(1, 11, 2);
      if (btitle.length() > 0) {
        tft.setTextColor(0xD484);
        tft.print(btitle);
        tft.print(" ");
      }
      if (bnick.length() > 0) {
        tft.setTextColor(TFT_WHITE);
        tft.println(bnick);
      }
      drawClock(tft, bclock);
      if (brating) {
        tft.setCursor(85, 27, 2);
        tft.setTextColor(TFT_GREY);
        tft.print("(");
        tft.print(brating);
        tft.print(")");
      }

      tft.setCursor(1, 175, 2);
      if (wtitle.length() > 0) {
        tft.setTextColor(0xD484);
        tft.print(wtitle);
        tft.print(" ");
      }
      if (wnick.length() > 0) {
        tft.setTextColor(TFT_WHITE);
        tft.println(wnick);
      }
      drawClock(tft, wclock);
      if (wrating) {
        tft.setCursor(85, 191, 2);
        tft.setTextColor(TFT_GREY);
        tft.print("(");
        tft.print(wrating);
        tft.print(")");
      }
    }

    void drawTime(TFT_eSPI tft) {
      tft.setCursor(0, 27, 2);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      bool whiteTurn = ply % 2 == 0;
      float bupdatedClock = whiteTurn ? bclock : max(0.0, bclock - (millis() - lastUpdate) / 1000.0);
      drawClock(tft, bupdatedClock);
      tft.setCursor(0, 191, 2);
      float wupdatedClock = whiteTurn ? max(0.0, wclock - (millis() - lastUpdate) / 1000.0) : wclock;
      drawClock(tft, wupdatedClock);
    }

    static void drawClock(TFT_eSPI tft, float clk) {
      int mins = (int)clk / 60;
      float secs = clk - (mins * 60);
      tft.printf("%02d", mins);
      tft.print(":");
      tft.printf("%05.2f", secs);
    }

    void connectGame(TFT_eSPI tft) {
      wc.close();
      String gameId = getGame(tft);
      if (gameId.isEmpty()) return;
      String socket = String("wss://socket1.lichess.org/watch/") + gameId + "/white/v5?sri=f32";
      Serial.println(socket);
      tft.println("Found game");

      tft.println("Getting game data");
      getGameInfo(tft, gameId);

      tft.println("Connecting to stream");
      wc.addHeader("origin", "https://lichess.org");
      wc.connect(socket);
    }

    static String getGame(TFT_eSPI tft) {
      WiFiClientSecure *clientHttps = new WiFiClientSecure;
      // Checking this cert causes random crashes :(
      //clientHttps->setCACert(letsEncryptCert);
      HTTPClient https;
      if (https.begin(*clientHttps, "https://lichess.org/tv/channels")) {
        int httpCode = https.GET();

        delay(1);
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();

            DynamicJsonDocument doc(2000);
            delay(1);
            DeserializationError err = deserializeJson(doc, payload);
            String s = doc["Blitz"]["gameId"];
            return s;
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
      if (clientHttps != nullptr) delete clientHttps;
    }

    void getGameInfo(TFT_eSPI tft, String gameId) {
      //Serial.println("Getting game info");
      WiFiClientSecure *clientHttps = new WiFiClientSecure;
      // Checking this cert causes random crashes :(
      //clientHttps->setCACert(letsEncryptCert);
      HTTPClient https;
      if (https.begin(*clientHttps, String("https://lichess.org/game/export/") + gameId)) {
        https.addHeader("Accept", "application/json");
        int httpCode = https.GET();

        delay(1);
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();

            DynamicJsonDocument doc(2000);
            delay(1);
            DeserializationError err = deserializeJson(doc, payload);
            JsonObject white = doc["players"]["white"];
            JsonObject black = doc["players"]["black"];

            wrating = white["rating"].as<int>();
            wnick = white["user"]["name"].as<String>();
            wtitle = white["user"].containsKey("title") ? white["user"]["title"].as<String>() : "";

            brating = black["rating"].as<int>();
            bnick = black["user"]["name"].as<String>();
            btitle = black["user"].containsKey("title") ? black["user"]["title"].as<String>() : "";
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
      if (clientHttps != nullptr) delete clientHttps;
    }

    void resetGame() {
      gameState = LOADING;
      shouldRender = false;
      ply = 0;
      isCheck = false;
    }

    void onMessage(WebsocketsMessage msg) {
      Serial.println("Got Message: " + msg.data());
      DynamicJsonDocument doc(2500);
      delay(1);
      DeserializationError err = deserializeJson(doc, msg.data());
      if (err) {
        Serial.print("Error deserialising: ");
        Serial.println(err.c_str());
      }
      String t = doc["t"];
      if (t.equals("move")) {
        clearBoard();
        const char* newFen = doc["d"]["fen"];
        sprintf(fen, "%.99s", newFen);
        //Serial.print("Got FEN: ");
        //Serial.println(fen);
        parseFen();
        //Serial.println("Parsed");

        JsonObject clk = doc["d"]["clock"];
        wclock = clk["white"].as<float>();
        bclock = clk["black"].as<float>();

        ply = doc["d"]["ply"];
        isCheck = doc["d"]["check"];

        const char* uci = doc["d"]["uci"];
        byte rank;
        byte file;
        rank = (uci[0] - 'a');
        file = 7 - (uci[1] - '1');
        lastMoveFrom =  (rank + file * 8);
        rank = (uci[2] - 'a');
        file = 7 - (uci[3] - '1');
        lastMoveTo =  (rank + file * 8);

        lastUpdate = millis();
        shouldRender = true;
      } else if (t.equals("endData")) {
        if (doc["d"]["winner"].isNull()) {
          gameState = DRAW;
        } else if (doc["d"]["winner"].as<String>().equals("white")) {
          gameState = WHITE_WIN;
        } else {
          gameState = BLACK_WIN;
        }
        resetSleepTimer();
        wc.close();
        shouldRender = true;
      } else {
        wc.send("null");
      }
    }

  public:
    char* getMenuName() override {
      return "Lichess TV";
    }

    void onSetup(TFT_eSPI tft) override {
      resetGame();
      fillScreen(tft, TFT_BLACK);
      tft.setCursor(1, 11, 2);

      if (!eventsRegistered) {
        wc.setCACert(letsEncryptCert);

        Serial.println("Setting event");
        wc.onMessage([&](WebsocketsMessage msg) {
          onMessage(msg);
        });

        wc.onEvent([&](WebsocketsEvent event, String data) {
          if (event == WebsocketsEvent::ConnectionOpened) {
            Serial.println("Connnection Opened");
            gameState = GAME_ON;
            wc.ping();
          } else if (event == WebsocketsEvent::ConnectionClosed) {
            Serial.println("Connnection Closed");
            int reason = wc.getCloseReason();
            Serial.println(reason);
            if (reason > 1000) {
              gameState = DISCONNECTED;
              shouldRender = true;
            }
          } else if (event == WebsocketsEvent::GotPing) {
          } else if (event == WebsocketsEvent::GotPong) {
          }
        });
        eventsRegistered = true;
      }
      tft.println("Connecting...");
      connectGame(tft);
    };

    void render(TFT_eSPI tft) override {
      if (wc.available()) wc.poll();

      if (gameState == DRAW || gameState == WHITE_WIN || gameState == BLACK_WIN) {
        if (shouldRender) {
          fillScreen(tft, TFT_BLACK);
          drawUsers(tft);
          drawBoard(tft);
          if (gameState == DRAW) {
            tft.drawLine(0, 25, 135, 25, 0xD484);
            tft.drawLine(0, 189, 135, 189, 0xD484);
          } else if (gameState == WHITE_WIN) {
            tft.drawLine(0, 189, 135, 189, TFT_GREEN);
          } else {
            tft.drawLine(0, 25, 135, 25, TFT_GREEN);
          }
          shouldRender = false;
        }
      } else if (gameState == GAME_ON) {
        if (!shouldRender) {
          drawTime(tft);
          vTaskDelay(100);
        } else {
          fillScreen(tft, TFT_BLACK);
          drawUsers(tft);
          drawBoard(tft);
          shouldRender = false;
          vTaskDelay(100);
        }
      } else if (gameState == DISCONNECTED) {
        if (shouldRender) {
          fillScreen(tft, TFT_BLACK);
          tft.setCursor(1, 30, 2);
          tft.println("Got disconnected");
          shouldRender = false;
        }
      } else if (gameState == RELOAD) {
        return onSetup(tft);
      }
    };

    void onButton1LongClick() override {
      gameState = RELOAD;
    };

    void onClose() override {
      gameState = DISCONNECTED;
      Serial.println("Closing");
      wc.close();
    }

    bool getSleepDisabled() override {
      return (gameState == LOADING || gameState == GAME_ON);
    };

    const unsigned short* getIcon() override {
      return lichesslogo;
    };
};

LichessTv lichesstv;
