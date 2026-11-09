// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
#pragma once

// ============================================================================
//  SNAKE
// ============================================================================
namespace Snake {
  const int CS = 10, OY = 20, MAXW = 32, MAXH = 32;
  const int DX[4] = {0, 1, 0, -1};
  const int DY[4] = {-1, 0, 1, 0};
  const uint16_t START_IV[3] = {190, 140, 100};
  const uint16_t MIN_IV[3]   = {80, 55, 40};

  // Spielfeld: Bit 7 = Schlange liegt hier, Bits 0-1 = Richtung zum naechsten
  // Glied (Richtung Kopf). So reicht 1 Byte pro Feld statt einer Positionsliste.
  int GW, GH;
  GArr<uint8_t[MAXH], MAXW> grid;
  int tx, ty, hx, hy, len, dir, nextDir, fx, fy;
  uint32_t score, lastStep;
  uint16_t interval;

  bool occupied(int x, int y) { return grid[x][y] & 0x80; }

  void cell(int x, int y, uint16_t c) {
    tft.fillRect(x * CS + 1, OY + y * CS + 1, CS - 2, CS - 2, c);
  }

  void drawHud() {
    char buf[24];
    snprintf(buf, sizeof(buf), "SCORE %lu", (unsigned long)score);
    hud(buf, 6, 6, 120, 1, C_WHITE, C_BG);
    snprintf(buf, sizeof(buf), "HI %lu", (unsigned long)highscore[curGame]);
    hudR(buf, SCR_W - 6, 6, 90, 1, C_YELLOW, C_BG);
  }

  void drawFood() {
    tft.fillCircle(fx * CS + CS / 2, OY + fy * CS + CS / 2, 4, C_RED);
    tft.drawPixel(fx * CS + CS / 2 + 1, OY + fy * CS + 2, C_GREEN);
  }

  void placeFood() {
    do { fx = random(GW); fy = random(GH); } while (occupied(fx, fy));
    drawFood();
  }

  void drawAll() {
    tft.fillScreen(C_BG);
    tft.drawFastHLine(0, OY - 1, SCR_W, C_DIM);
    int x = tx, y = ty;
    for (int i = 0; i < len; i++) {
      cell(x, y, i == len - 1 ? C_GREEN : C_DGREEN);
      int d = grid[x][y] & 3;
      x += DX[d]; y += DY[d];
    }
    if (fx >= 0) drawFood();
    drawHud();
  }

  void start() {
    grid.alloc();
    GW = min(MAXW, SCR_W / CS);
    GH = min(MAXH, (SCR_H - OY) / CS);
    memset(grid.p, 0, grid.bytes());
    len = 4;
    tx = GW / 2 - 4; ty = GH / 2;
    for (int i = 0; i < len; i++) grid[tx + i][ty] = 0x80 | 1;
    hx = tx + len - 1; hy = ty;
    dir = nextDir = 1;
    score = 0;
    interval = START_IV[cfg.snakeSpeed];
    lastStep = millis();
    fx = fy = -1;
    drawAll();
    placeFood();
  }

  void step() {
    dir = nextDir;
    int nx = hx + DX[dir];
    int ny = hy + DY[dir];
    if (nx < 0 || nx >= GW || ny < 0 || ny >= GH) { gameOver(score); return; }

    bool eat = (nx == fx && ny == fy);
    if (!eat) {              // Schwanz einziehen
      int d = grid[tx][ty] & 3;
      grid[tx][ty] = 0;
      cell(tx, ty, C_BG);
      tx += DX[d]; ty += DY[d];
      len--;
    }
    if (occupied(nx, ny)) { gameOver(score); return; }

    grid[hx][hy] = 0x80 | dir;            // alter Kopf zeigt auf den neuen
    cell(hx, hy, C_DGREEN);
    hx = nx; hy = ny;
    grid[hx][hy] = 0x80 | dir;
    len++;
    cell(hx, hy, C_GREEN);

    if (eat) {
      score += 10;
      if (interval > MIN_IV[cfg.snakeSpeed]) interval -= 3;
      rumble(60, 0x60);
      drawHud();
      if (len >= GW * GH) { gameOver(score + 1000); return; }
      placeFood();
    }
  }

  void loop() {
    if (handlePause(drawAll)) return;
    int want = -1;
    if (cur.up) want = 0;
    else if (cur.right) want = 1;
    else if (cur.down) want = 2;
    else if (cur.left) want = 3;
    if (want >= 0 && want != (dir + 2) % 4) nextDir = want;

    if (millis() - lastStep >= interval) {
      lastStep = millis();
      step();
    }
  }
}
