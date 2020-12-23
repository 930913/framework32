// Adapted from Volos Projects' code: https://www.youtube.com/watch?v=N6V7ZJkhSbc

#include "breakoutlogo.h"
#include "breakoutsplash.h"
#define TFT_GREY 0x5AEB
#define lightblue 0x2D18
#define orange 0xFB60
#define purple 0xFB9B

class Breakout : public App {
  private:
    float ys = 1;

    float x = random(30, 100); //coordinates of ball
    float y = 70;
    int ny = y; //coordinates of previous position
    int nx = x;

    float px = 45; //67 je sredina    pozicija igrača
    int   pxn = px;
    int vrije[2] = {1, -1};
    int enx[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
    int eny[16] = {37, 37, 37, 37, 37, 45, 45, 45, 45, 45, 53, 53, 53, 53, 61, 61};
    int enc[16] = {TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, orange, orange, orange, orange, lightblue, lightblue};
    int score = 0;
    int level = 1;
    float amount[4] = {0.25, 0.50, 0.75, 1};
    float xs = amount[random(4)] * vrije[random(2)];
    int fase = 0;


    float xstep = 1;
    int spe = 0;
    int pom = 0;
    int gameSpeed = 7000;
    bool gameover = false;
    void newLevel(TFT_eSPI tft);

  public:
    void onSetup(TFT_eSPI& tft) override {
      tft.pushImage(0, 0,  135, 240, breakoutsplash);
      ys = 1;

      x = random(30, 100); //coordinates of ball
      y = 70;
      ny = y; //coordinates of previous position
      nx = x;

      px = 45; //67 je sredina    pozicija igrača
      pxn = px;
      //  vrije[2]={1,-1};
      //  enx[16]={8,33,58,83,108,8,33,58,83,108,22,47,72,97,47,72};
      //  eny[16]={37,37,37,37,37,45,45,45,45,45,53,53,53,53,61,61};
      //  enc[16]={TFT_RED,TFT_RED,TFT_RED,TFT_RED,TFT_RED,TFT_GREEN,TFT_GREEN,TFT_GREEN,TFT_GREEN,TFT_GREEN,orange,orange,orange,orange,lightblue,lightblue};
      score = 0;
      level = 1;
      //  amount[4]={0.25,0.50,0.75,1};
      xs = amount[random(4)] * vrije[random(2)];
      fase = 0;
      xstep = 1;
      spe = 0;
      pom = 0;
      gameSpeed = 7000;
      gameover = false;

      newLevel(tft);
      score = 0;
      level = 1;
    }

    void render(TFT_eSPI& tft) override {

      if (fase == 0)
      {

        if (digitalRead(0) == 0 || digitalRead(35) == 0)
        {
          if (pom == 0) {
            tft.fillScreen(TFT_BLACK);
            tft.drawLine(0, 17, 0, 240, TFT_GREY);
            tft.drawLine(0, 17, 135, 17, TFT_GREY);
            tft.drawLine(134, 17, 134, 240, TFT_GREY);

            tft.setCursor(3, 3, 2);

            tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);

            tft.setCursor(0, 0, 2);
            tft.println("SCORE " + String(score));

            tft.setCursor(99, 0, 2);
            tft.println("LVL" + String(level));
            fase = fase + 1;
            pom = 1;
          }

        } else {
          pom = 0;
        }
      }

      if (fase == 1) {

        if (y != ny) {
          tft.fillEllipse(nx, ny, 2, 2, TFT_BLACK);   //brisanje loptice
          ny = y;
          nx = x;
        }
        if (int(px) != pxn) {
          tft.fillRect(pxn, 234, 24, 4, TFT_BLACK); //briasnje igraca
          pxn = px;
        }


        // spe=spe+1;

        if (px >= 2 && px <= 109) {
          if (digitalRead(0) == 0)
            px = px - 1;
          if (digitalRead(35) == 0)
            px = px + 1;
        }
        if (px <= 3)
          px = 4;

        if (px >= 108)
          px = 107;

        if (y > 232 && x > px && x < px + 24) { ///brisati kasnije
          ys = ys * -1;

          xs = amount[random(4)] * vrije[random(2)];


        }

        for (int j = 0; j < 16; j++)
        {
          if (x > enx[j] && x < enx[j] + 20 && y > eny[j] && y < eny[j] + 5)
          {
            tft.fillRect(enx[j], eny[j], 20, 4, TFT_BLACK);
            enx[j] = 400;
            ys = ys * -1;
            xs = amount[random(4)] * vrije[random(2)];

            score = score + 1;

            tft.setCursor(0, 0, 2);
            tft.println("SCORE " + String(score));
          }
        }

        if (y < 21)
          ys = ys * -1.00;

        if (y > 240)
          fase = fase + 1;

        if (x >= 130)
          xs = xs * -1.00;

        if (x <= 4)
          xs = xs * -1.00;


        for (int i = 0; i < 16; i++) //draw enemies
          tft.fillRect(enx[i], eny[i], 20, 4, enc[i]);


        tft.fillEllipse(int(x), y, 2, 2, TFT_WHITE); // draw ball

        //if(spe>10){  //change coordinates of ball
        y = y + ys;
        x = x + xs;
        //spe=0;
        //}

        tft.fillRect(px, 234, 24, 4, TFT_WHITE);

        if (score == 16 || score == 33 || score == 50 || score == 67 || score == 84 || score == 101 || score == 118 || score == 135 || score == 152 || score == 169)
          newLevel(tft);

        delayMicroseconds(gameSpeed);


      }
      if (fase == 2)
      {
        if (gameover) return;

        tft.fillScreen(TFT_BLACK);

        tft.setCursor(13, 103, 2);

        tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);

        tft.println("GAME OVER");
        tft.setCursor(13, 123, 4);
        tft.println("SCORE:" + String(score));

        tft.setCursor(13, 153, 4);
        tft.println("LEVEL:" + String(level));

        tft.setCursor(13, 123, 4);
        tft.println("SCORE:" + String(score));

        gameover = true;

        //delay(3000);
      }
    }

    bool getFullscreen() override {
      return true;
    }
    char* getMenuName() override {
      return "Breakout";
    }
    const unsigned short* getIcon() override {
      return breakoutlogo;
    };
};

Breakout breakout;
