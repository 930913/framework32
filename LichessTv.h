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
    enum GameState { SELECT_TYPE, LOADING, GAME_ON, WHITE_WIN, BLACK_WIN, DRAW, DISCONNECTED, RELOAD };
    GameState gameState;
    int ply;
    bool isCheck;
    byte menuOffset;
    byte menuSelection;
    const char* gameTypes[17] = { "Top Rated", "Blitz", "Bullet", "Rapid", "UltraBullet", "Antichess", "Horde", "Atomic", "Crazyhouse", "Chess960", "King of the Hill", "Racing Kings", "Classical", "Three-check", "Bot", "Computer" };
    byte gameType = 0;

    static const unsigned long lightSquareColour = 0xE695;
    static const unsigned long darkSquareColour = 0xB44C;
    static const unsigned long movedLightSquareColour = 0xCE8D;
    static const unsigned long movedDarkSquareColour = 0xAD07;

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
      char* square = board;
      char* ptr = fen;
      while (*ptr) {
        if (*ptr > 60) {
          *square++ = *ptr++;
        } else if (*ptr == '/') {
          ++ptr; // Ignore? Assumes full FEN
        } else {
          square += *ptr++ - '0';
        }
      }
    }

    void drawBoard(TFT_eSPI tft) {
      //Serial.println("Drawing board");
      int boardOffsetX = max(0, Util::Screen::isPortrait() ? (Util::Screen::Grid::x1m1() - 64) : (Util::Screen::Grid::x2m1() - 64));
      int boardOffsetY = Util::Screen::Grid::y1m1() - 64;
      for (int i = 0; i < 64; ++i) {
        byte file = i % 8;
        byte rank = i / 8;
        int x = file * 16 + boardOffsetX;
        int y = (rank * 16) + boardOffsetY;
        bool isLight = (rank + (file % 2) + 1) % 2;
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
      tft.setCursor(Util::Screen::isPortrait() ? Util::Screen::Grid::x1l1() : 130, Util::Screen::Grid::y2t1() + 1, 2);
      if (btitle.length() > 0) {
        tft.setTextColor(0xD484);
        tft.print(btitle);
        tft.print(" ");
      }
      if (bnick.length() > 0) {
        tft.setTextColor(TFT_WHITE);
        tft.print(bnick);
      }
      if (brating) {
        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(TFT_GREY);
        tft.drawString(String("(") + brating + ")", Util::Screen::Grid::x1r1() - 1, Util::Screen::Grid::y2t1() + 16, 2);
        tft.setTextDatum(MC_DATUM);
      }

      tft.setCursor(Util::Screen::isPortrait() ? Util::Screen::Grid::x1l1() : 130, Util::Screen::Grid::y2b2() - 40, 2);
      if (wtitle.length() > 0) {
        tft.setTextColor(0xD484);
        tft.print(wtitle);
        tft.print(" ");
      }
      if (wnick.length() > 0) {
        tft.setTextColor(TFT_WHITE);
        tft.print(wnick);
      }
      if (wrating) {
        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(TFT_GREY);
        tft.drawString(String("(") + wrating + ")", Util::Screen::Grid::x1r1() - 1, Util::Screen::Grid::y2b2() - 25, 2);
        tft.setTextDatum(MC_DATUM);
      }
      drawTime(tft);
    }

    void drawTime(TFT_eSPI tft) {
      tft.setCursor(Util::Screen::isPortrait() ? Util::Screen::Grid::x1l1() : 130, Util::Screen::Grid::y2t1() + 16, 2);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      bool whiteTurn = ply % 2 == 0;
      float bupdatedClock = whiteTurn ? bclock : max(0.0, bclock - (millis() - lastUpdate) / 1000.0);
      drawClock(tft, bupdatedClock);
      tft.setCursor(Util::Screen::isPortrait() ? Util::Screen::Grid::x1l1() : 130, Util::Screen::Grid::y2b2() - 25, 2);
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

    void drawMenu(TFT_eSPI tft) {
      Serial.println(menuSelection);
      Serial.println(menuOffset);
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setCursor(0, 11, 2);
      for(int i = menuOffset; i < 17 && i < menuOffset + 14; ++i) {
        tft.print(gameTypes[i]);
        if(i == menuSelection) tft.print(" <");
        tft.println();
      }
      shouldRender = false;
    }

    void drawHamburger(TFT_eSPI tft) {
      tft.drawLine(25, 235, 30, 235, TFT_WHITE);
      tft.drawLine(25, 237, 30, 237, TFT_WHITE);
      tft.drawLine(25, 239, 30, 239, TFT_WHITE);
    }

    void drawReload(TFT_eSPI tft) {
      // TODO: Use a better icon
      tft.drawCircle(110, 236, 2, TFT_WHITE);
    }

    void drawIcons(TFT_eSPI tft) {
      drawHamburger(tft);
      drawReload(tft);
    }

    void connectGame(TFT_eSPI tft) {
      wc.close();
      String gameId = getGame(tft, gameTypes[gameType]);
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

    static String getGame(TFT_eSPI tft, String gameType) {
      WiFiClientSecure *clientHttps = new WiFiClientSecure;
      // Checking this cert causes random crashes :(
      //clientHttps->setCACert(letsEncryptCert);
      HTTPClient https;
      if (https.begin(*clientHttps, "https://lichess.org/api/tv/channels")) {
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
            String s = doc[gameType]["gameId"];
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

    void onSetup(TFT_eSPI& tft) override {
      resetGame();
      Util::Screen::fillScreen(tft, TFT_BLACK);
      tft.setCursor(1, 11, 2);

      if (!eventsRegistered) {
        wc.setCACert(dstRootCert);

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

    void render(TFT_eSPI& tft) override {
      if (wc.available()) wc.poll();

      if (gameState == SELECT_TYPE) {
        if (shouldRender) drawMenu(tft);
      } else if (gameState == DRAW || gameState == WHITE_WIN || gameState == BLACK_WIN) {
        if (shouldRender) {
          Util::Screen::fillScreen(tft, TFT_BLACK);
          drawUsers(tft);
          drawBoard(tft);
          drawIcons(tft);
          int by = Util::Screen::Grid::y2t1() + 15;
          int wy = Util::Screen::Grid::y2b2() - 26;
          int x1 = Util::Screen::isPortrait() ? Util::Screen::Grid::x1l1() : Util::Screen::Grid::x2l2();
          int x2 = Util::Screen::isPortrait() ? Util::Screen::Grid::x1r1() : Util::Screen::Grid::x2r2();
          if (gameState == DRAW) {
            tft.drawLine(x1, by, x2, by, 0xD484);
            tft.drawLine(x1, wy, x2, wy, 0xD484);
          } else if (gameState == WHITE_WIN) {
            tft.drawLine(x1, wy, x2, wy, TFT_GREEN);
          } else {
            tft.drawLine(x1, by, x2, by, TFT_GREEN);
          }
          shouldRender = false;
        }
      } else if (gameState == GAME_ON) {
        if (!shouldRender) {
          drawTime(tft);
          vTaskDelay(100);
        } else {
          Util::Screen::fillScreen(tft, TFT_BLACK);
          drawUsers(tft);
          drawBoard(tft);
          drawIcons(tft);
          shouldRender = false;
          vTaskDelay(100);
        }
      } else if (gameState == DISCONNECTED) {
        if (shouldRender) {
          Util::Screen::fillScreen(tft, TFT_BLACK);
          tft.setCursor(1, 30, 2);
          tft.println("Got disconnected");
          drawIcons(tft);
          shouldRender = false;
        }
      } else if (gameState == RELOAD) {
        return onSetup(tft);
      }
    };

    void onButton1LongClick() override {
      if(gameState == SELECT_TYPE) {
        gameType = menuSelection;
        gameState = RELOAD;
      } else {
        gameState = RELOAD;
      }
    };

    void onButton2Click() override {
      if(gameState == SELECT_TYPE) {
        menuSelection = max(0, menuSelection - 1);
        if(menuSelection < menuOffset) --menuOffset;
        shouldRender = true;
      } else {
        wc.close();
        gameState = SELECT_TYPE;
        shouldRender = true;
      }
    };

    void onButton1Click() override {
      if(gameState == SELECT_TYPE) {
        menuSelection = min(15, menuSelection + 1);
        if(menuSelection > menuOffset + 13) ++menuOffset;
        shouldRender = true;
      }
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
