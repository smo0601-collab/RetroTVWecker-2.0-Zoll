// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
#pragma once

// ============================================================================
//  BRICKS (Mauer-Ausbrecher)
// ============================================================================
namespace Bricks {
  const int COLS = 10, ROWS = 6, BHGT = 12, BTOP = 34, TOP = 20, PH = 6, BS = 6;
  const uint16_t ROWCOL[ROWS] = {C_RED, C_ORANGE, C_YELLOW, C_GREEN, C_CYAN, C_BLUE};

  int BWID, PW, PY;
  uint8_t bricks[ROWS][COLS];
  int remaining, lives, level;
  uint32_t score;
  float bx, by, vx, vy, padX, speed;
  bool stuck;
  int oBx, oBy, oPad;

  void drawHud() {
    char buf[24];
    snprintf(buf, sizeof(buf), "SCORE %lu", (unsigned long)score);
    hud(buf, 6, 6, 100, 1, C_WHITE, C_BG);
    snprintf(buf, sizeof(buf), "LV %d", level + 1);
    hud(buf, SCR_W / 2 - 10, 6, 40, 1, C_CYAN, C_BG);
    tft.fillRect(SCR_W - 50, 3, 50, 13, C_BG);
    for (int i = 0; i < lives; i++) tft.fillCircle(SCR_W - 10 - i * 12, 9, 4, C_PINK);
  }

  void drawBrick(int r, int c) {
    int x = c * BWID, y = BTOP + r * BHGT;
    if (bricks[r][c]) {
      tft.fillRect(x + 1, y + 1, BWID - 2, BHGT - 2, ROWCOL[bricks[r][c] - 1]);
      tft.drawFastHLine(x + 1, y + 1, BWID - 2, C_WHITE);
    } else {
      tft.fillRect(x, y, BWID, BHGT, C_BG);
    }
  }

  void drawAll() {
    tft.fillScreen(C_BG);
    tft.drawFastHLine(0, TOP - 1, SCR_W, C_DIM);
    for (int r = 0; r < ROWS; r++)
      for (int c = 0; c < COLS; c++) drawBrick(r, c);
    drawHud();
    oBx = oBy = oPad = -1000;
  }

  int sdLevelCount() {
    if (!sdOk) return 0;
    char path[48];
    int n = 0;
    while (n < 99) {
      snprintf(path, sizeof(path), "%s/level%d.txt", SD_LVLDIR, n + 1);
      if (!SD.exists(path)) break;
      n++;
    }
    return n;
  }

  bool loadLevelFromSD(int lvl) {
    int count = sdLevelCount();
    if (count == 0) return false;
    char path[48];
    snprintf(path, sizeof(path), "%s/level%d.txt", SD_LVLDIR, (lvl % count) + 1);
    File f = SD.open(path, FILE_READ);
    if (!f) return false;
    memset(bricks, 0, sizeof(bricks));
    remaining = 0;
    for (int r = 0; r < ROWS && f.available(); r++) {
      String line = f.readStringUntil('\n');
      for (int c = 0; c < COLS && c < (int)line.length(); c++) {
        char ch = line[c];
        if (ch >= '1' && ch <= '6') { bricks[r][c] = ch - '0'; remaining++; }
      }
    }
    f.close();
    return remaining > 0;
  }

  void setupLevel() {
    if (!loadLevelFromSD(level)) {
      remaining = 0;
      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) { bricks[r][c] = r + 1; remaining++; }
    }
    speed = 3.0f + level * 0.4f;
    stuck = true;
    drawAll();
  }

  void start() {
    BWID = SCR_W / COLS;
    PW = landscape() ? 50 : 42;
    PY = SCR_H - 18;
    score = 0; lives = 3; level = 0;
    padX = (SCR_W - PW) / 2.0f;
    setupLevel();
  }

  bool hitBrick() {
    int pts[4][2] = {{(int)bx, (int)by}, {(int)bx + BS - 1, (int)by},
                     {(int)bx, (int)by + BS - 1}, {(int)bx + BS - 1, (int)by + BS - 1}};
    for (auto& p : pts) {
      int x = p[0], y = p[1];
      if (y < BTOP) continue;
      int c = x / BWID, r = (y - BTOP) / BHGT;
      if (r < ROWS && c >= 0 && c < COLS && bricks[r][c]) {
        bricks[r][c] = 0;
        remaining--;
        drawBrick(r, c);
        score += (ROWS - r) * 10;
        drawHud();
        Snd::fx(Snd::BRICK);
        rumble(40, 0x40);
        return true;
      }
    }
    return false;
  }

  void loseLife() {
    lives--;
    Snd::fx(Snd::DIE);
    rumble(300, 0xA0);
    drawHud();
    if (lives <= 0) { gameOver(score); return; }
    stuck = true;
  }

  void loop() {
    if (handlePause(drawAll)) return;

    if (cur.left)  padX -= 6;
    if (cur.right) padX += 6;
    if (abs(cur.ax) > 60) padX += cur.ax / 512.0f * 4.0f;
    padX = constrain(padX, 0.0f, (float)(SCR_W - PW));

    if (stuck) {
      bx = padX + PW / 2.0f - BS / 2.0f;
      by = PY - BS - 1;
      if (PRESSED(a)) {
        stuck = false;
        float ang = (random(2) ? 1 : -1) * radians(25 + random(20));
        vx = speed * sinf(ang);
        vy = -speed * cosf(ang);
      }
    } else {
      int steps = (int)ceilf(max(fabsf(vx), fabsf(vy)) / 2.0f);
      if (steps < 1) steps = 1;
      for (int s = 0; s < steps && !stuck; s++) {
        float sx = vx / steps, sy = vy / steps;
        bx += sx;
        if (bx < 0) { bx = 0; vx = fabsf(vx); Snd::fx(Snd::WALL, Snd::P_AUTO); }
        if (bx > SCR_W - BS) { bx = SCR_W - BS; vx = -fabsf(vx); Snd::fx(Snd::WALL, Snd::P_AUTO); }
        if (hitBrick()) { bx -= sx; vx = -vx; }
        by += sy;
        if (by < TOP) { by = TOP; vy = fabsf(vy); Snd::fx(Snd::WALL, Snd::P_AUTO); }
        if (hitBrick()) { by -= sy; vy = -vy; }

        if (vy > 0 && by + BS >= PY && by + BS <= PY + PH + 3 &&
            bx + BS >= padX && bx <= padX + PW) {
          float hit = ((bx + BS / 2.0f) - (padX + PW / 2.0f)) / (PW / 2.0f);
          hit = constrain(hit, -1.0f, 1.0f);
          float ang = hit * 1.05f;
          vx = speed * sinf(ang);
          vy = -speed * cosf(ang);
          by = PY - BS;
          Snd::fx(Snd::BOUNCE);
        }
        if (by > SCR_H) {
          tft.fillRect(oBx, oBy, BS, BS, C_BG);
          oBx = oBy = -1000;
          loseLife();
          if (state != ST_GAME) return;
        }
      }
      if (remaining == 0) {
        level++;
        score += 500;
        Snd::fx(Snd::WIN);
        rumble(250, 0x80);
        setupLevel();
        return;
      }
    }

    int ip = (int)padX;
    if (ip != oPad) {
      if (oPad > -1000) tft.fillRect(oPad, PY, PW, PH, C_BG);
      tft.fillRoundRect(ip, PY, PW, PH, 3, C_WHITE);
      tft.drawFastHLine(ip + 3, PY + PH - 1, PW - 6, C_GRAY);
      oPad = ip;
    }
    int ix = (int)bx, iy = (int)by;
    if (ix != oBx || iy != oBy) {
      if (oBx > -1000) tft.fillRect(oBx, oBy, BS, BS, C_BG);
      tft.fillRect(ix, iy, BS, BS, C_YELLOW);
      oBx = ix; oBy = iy;
    }
  }
}
