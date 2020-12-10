void Breakout::newLevel(TFT_eSPI tft) {
  score = score + 1;
  delay(3000);
  gameSpeed = gameSpeed - 500;
  level = level + 1;
  tft.setCursor(99, 0, 2);
  tft.println("LVL" + String(level));
  y = 75;
  ys = 1;
  x = random(30, 100);

  int enx2[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
  for (int n = 0; n < 16; n++) {
    enx[n] = enx2[n];
  }
};
