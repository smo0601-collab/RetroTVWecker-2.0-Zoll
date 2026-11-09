// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
#pragma once

// ============================================================================
//  BLOCKS (Fallende-Steine-Puzzle)
// ============================================================================
namespace Blocks {
  const int BW = 10, BH = 20, NS = 11;
  int CS, OX, OY, statX, statY0, statDY, nextX, nextY, helpX, helpY;

  const uint16_t SHAPES[7][4] = {
    {0x0F00, 0x2222, 0x00F0, 0x4444},  // I
    {0x44C0, 0x8E00, 0x6440, 0x0E20},  // J
    {0x4460, 0x0E80, 0xC440, 0x2E00},  // L
    {0xCC00, 0xCC00, 0xCC00, 0xCC00},  // O
    {0x06C0, 0x8C40, 0x6C00, 0x4620},  // S
    {0x0E40, 0x4C40, 0x4E00, 0x4640},  // T
    {0x0C60, 0x4C80, 0xC600, 0x2640},  // Z
  };
  const uint16_t COLORS[7] = {C_CYAN, C_BLUE, C_ORANGE, C_YELLOW, C_GREEN, C_PURPLE, C_RED};

  uint8_t board[BH][BW];
  uint8_t shown[BH][BW];
  int pt, pr, px, py, nextT;
  uint32_t score, lastFall;
  int lines, level;
  Repeat rl, rr, rd;

  void layout() {
    if (landscape()) {
      CS = 11; OX = 105; OY = 10;
      statX = 8;   statY0 = 20;  statDY = 42;
      nextX = 234; nextY = 20;
      helpX = 228; helpY = 160;
    } else {
      CS = 14; OX = 6; OY = (SCR_H - BH * CS) / 2;
      statX = 156; statY0 = 100; statDY = 36;
      nextX = 160; nextY = OY;
      helpX = 154; helpY = 252;
    }
  }

  bool fits(int t, int r, int x, int y) {
    uint16_t m = SHAPES[t][r];
    int row = 0, col = 0;
    for (uint16_t bit = 0x8000; bit; bit >>= 1) {
      if (m & bit) {
        int bx = x + col, by = y + row;
        if (bx < 0 || bx >= BW || by >= BH) return false;
        if (by >= 0 && board[by][bx]) return false;
      }
      if (++col == 4) { col = 0; row++; }
    }
    return true;
  }

  void block(int sxp, int syp, int size, uint8_t v) {
    tft.fillRect(sxp, syp, size, size, C_BG);
    if (v == 0) {
      tft.drawPixel(sxp + size / 2, syp + size / 2, C_GRID);
      return;
    }
    tft.fillRect(sxp + 1, syp + 1, size - 2, size - 2, COLORS[v - 1]);
    tft.drawFastHLine(sxp + 1, syp + 1, size - 2, C_WHITE);
    tft.drawFastVLine(sxp + 1, syp + 1, size - 2, C_WHITE);
  }

  void render() {
    uint8_t disp[BH][BW];
    memcpy(disp, board, sizeof(board));
    uint16_t m = SHAPES[pt][pr];
    int row = 0, col = 0;
    for (uint16_t bit = 0x8000; bit; bit >>= 1) {
      if (m & bit) {
        int bx = px + col, by = py + row;
        if (by >= 0 && by < BH && bx >= 0 && bx < BW) disp[by][bx] = pt + 1;
      }
      if (++col == 4) { col = 0; row++; }
    }
    for (int y = 0; y < BH; y++)
      for (int x = 0; x < BW; x++)
        if (disp[y][x] != shown[y][x]) {
          block(OX + x * CS, OY + y * CS, CS, disp[y][x]);
          shown[y][x] = disp[y][x];
        }
  }

  void drawNext() {
    tft.fillRect(nextX, nextY + 16, 4 * NS, 4 * NS, C_BG);
    uint16_t m = SHAPES[nextT][0];
    int row = 0, col = 0;
    for (uint16_t bit = 0x8000; bit; bit >>= 1) {
      if (m & bit) block(nextX + col * NS, nextY + 16 + row * NS, NS, nextT + 1);
      if (++col == 4) { col = 0; row++; }
    }
  }

  void drawStats() {
    char buf[16];
    uint32_t vals[4] = {score, (uint32_t)lines, (uint32_t)level, highscore[curGame]};
    for (int i = 0; i < 4; i++) {
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)vals[i]);
      hud(buf, statX, statY0 + i * statDY + 14, 82, 1.5, i == 3 ? C_YELLOW : C_WHITE, C_BG);
    }
  }

  void drawAll() {
    layout();
    tft.fillScreen(C_BG);
    tft.drawRect(OX - 2, OY - 2, BW * CS + 4, BH * CS + 4, C_GRAY);
    const char* labels[4] = {"SCORE", "LINES", "LEVEL", "HI"};
    for (int i = 0; i < 4; i++)
      txt(labels[i], statX, statY0 + i * statDY, 1, i == 3 ? C_YELLOW : C_CYAN, C_BG,
          textdatum_t::top_left);
    txt("NEXT", nextX, nextY, 1, C_CYAN, C_BG, textdatum_t::top_left);
    tft.drawRect(nextX - 4, nextY + 12, 4 * NS + 8, 4 * NS + 8, C_DIM);
    txt("A/B  drehen", helpX, helpY,      1, C_DIM, C_BG, textdatum_t::top_left);
    txt("hoch fallen", helpX, helpY + 14, 1, C_DIM, C_BG, textdatum_t::top_left);
    txt("Menu Pause",  helpX, helpY + 28, 1, C_DIM, C_BG, textdatum_t::top_left);
    memset(shown, 0xFF, sizeof(shown));
    drawNext();
    drawStats();
    render();
  }

  bool spawn() {
    pt = nextT;
    nextT = random(7);
    pr = 0; px = 3; py = 0;
    drawNext();
    return fits(pt, pr, px, py);
  }

  void lockPiece() {
    uint16_t m = SHAPES[pt][pr];
    int row = 0, col = 0;
    for (uint16_t bit = 0x8000; bit; bit >>= 1) {
      if (m & bit) {
        int by = py + row;
        if (by >= 0) board[by][px + col] = pt + 1;
      }
      if (++col == 4) { col = 0; row++; }
    }
    int cleared = 0;
    for (int y = BH - 1; y >= 0; y--) {
      bool full = true;
      for (int x = 0; x < BW; x++) if (!board[y][x]) { full = false; break; }
      if (full) {
        cleared++;
        for (int yy = y; yy > 0; yy--) memcpy(board[yy], board[yy - 1], BW);
        memset(board[0], 0, BW);
        y++;
      }
    }
    if (cleared) {
      const uint16_t pts[5] = {0, 100, 300, 500, 800};
      score += pts[cleared] * (level + 1);
      lines += cleared;
      level = lines / 10;
      rumble(80 + cleared * 60, 0x50 + cleared * 0x20);
    }
    drawStats();
    if (!spawn()) { render(); gameOver(score); return; }
    lastFall = millis();
  }

  void tryRotate(int dirR) {
    int nr = (pr + dirR + 4) % 4;
    const int kicks[5] = {0, -1, 1, -2, 2};
    for (int k : kicks)
      if (fits(pt, nr, px + k, py)) { pr = nr; px += k; return; }
  }

  void start() {
    memset(board, 0, sizeof(board));
    score = 0; lines = 0; level = 0;
    nextT = random(7);
    layout();
    spawn();
    lastFall = millis();
    drawAll();
  }

  void loop() {
    if (handlePause(drawAll)) return;

    if (rl.tick(cur.left, 170, 55)  && fits(pt, pr, px - 1, py)) px--;
    if (rr.tick(cur.right, 170, 55) && fits(pt, pr, px + 1, py)) px++;
    if (PRESSED(a)) tryRotate(1);
    if (PRESSED(b)) tryRotate(-1);

    if (PRESSED(up)) {
      int d = 0;
      while (fits(pt, pr, px, py + 1)) { py++; d++; }
      score += d * 2;
      render();
      lockPiece();
      return;
    }

    uint32_t interval = max(60, 750 - level * 65);
    bool soft = rd.tick(cur.down, 0, 40);
    if (soft || millis() - lastFall >= interval) {
      lastFall = millis();
      if (fits(pt, pr, px, py + 1)) {
        py++;
        if (soft) score += 1;
      } else {
        render();
        lockPiece();
        return;
      }
    }
    render();
  }
}
