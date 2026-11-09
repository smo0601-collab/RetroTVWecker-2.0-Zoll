// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  ROAD HOP  -  Ein Kueken ueberquert Strasse und Fluss bis zu seinem Nest
// ============================================================================
//  Steuerkreuz: je ein Hopser. Auf dem Fluss nur auf Baumstaemmen und
//  Schildkroeten stehen (abgetauchte Schildkroeten tragen nicht!).
//  Alle 5 Nester fuellen = naechstes Level.
// ============================================================================
#pragma once

namespace Hopper {
  const uint16_t CHICK[8] = {0x0038, 0x007C, 0x006E, 0x00FF, 0x007C, 0x003C, 0x0024, 0x006C};

  const int NR = 13;                     // Reihen: 0 Nester, 1-5 Fluss, 6 Rast, 7-11 Strasse, 12 Start
  enum { NONE, LOG, TURTLE, CAR, TRUCK };
  struct LaneDef { int8_t kind, len, count; float speed; uint16_t col; };
  const LaneDef LANES[NR] = {
    {NONE, 0, 0, 0, 0},
    {LOG, 4, 2, 0.8f, 0},       {TURTLE, 2, 3, -1.0f, 0}, {LOG, 5, 1, 1.3f, 0},
    {LOG, 3, 2, 0.6f, 0},       {TURTLE, 3, 2, -0.9f, 0},
    {NONE, 0, 0, 0, 0},
    {TRUCK, 2, 2, -0.8f, C_WHITE}, {CAR, 1, 1, 1.7f, C_PINK}, {CAR, 1, 3, -1.0f, C_YELLOW},
    {CAR, 1, 3, 1.1f, C_CYAN},     {CAR, 1, 3, -0.7f, C_ORANGE},
    {NONE, 0, 0, 0, 0},
  };

  float off[NR];
  int T, y0, cols;
  float hx; int hr, hopT, lastDir;
  int lives, level, deadT, timer, bestRow, winT;
  uint32_t score;
  bool homes[5];
  Repeat rUp, rDown, rLeft, rRight;

  float speedOf(int r) { return LANES[r].speed * T / 16.0f * (1.0f + level * 0.15f); }
  float spanW() { return SCR_W + 12.0f * T; }
  float objX(int r, int i) {
    float sp = spanW();
    float x = fmodf(off[r] + i * sp / LANES[r].count, sp);
    if (x < 0) x += sp;
    return x - 6 * T;
  }
  bool submerged(int r, int i) {
    if (LANES[r].kind != TURTLE || i != 0) return false;   // nur die erste Gruppe taucht
    return (tick + r * 53) % 210 > 165;
  }
  bool sinking(int r, int i) {
    if (LANES[r].kind != TURTLE || i != 0) return false;
    uint32_t p = (tick + r * 53) % 210;
    return p > 140 && p <= 165;
  }
  int rowY(int r) { return y0 + r * T; }
  float homeX(int k) { return SCR_W * (2 * k + 1) / 10.0f; }

  void resetChick() {
    hr = NR - 1;
    hx = ((cols / 2) * T);
    hopT = 0; bestRow = NR - 1;
    timer = 30 * 30;
  }

  void start() {
    T = min((SCR_H - 16 - 10) / NR, SCR_W / 12);
    y0 = 16;
    cols = SCR_W / T;
    for (int r = 0; r < NR; r++) off[r] = irand(0, 400);
    lives = 4; level = 0; score = 0; deadT = 0; winT = 0; lastDir = 0;
    for (auto& h : homes) h = false;
    resetChick();
  }

  void die() {
    if (deadT > 0) return;
    deadT = 40;
    lives--;
    rumble(400, 0xB0);
  }

  void landHome() {
    float c = hx + T / 2.0f;
    for (int k = 0; k < 5; k++) {
      if (fabsf(c - homeX(k)) < T * 0.85f) {
        if (homes[k]) break;
        homes[k] = true;
        score += 50 + timer / 3;
        rumble(150, 0x70);
        bool all = true;
        for (auto h : homes) all &= h;
        if (all) { score += 1000; winT = 60; }
        resetChick();
        return;
      }
    }
    die();
  }

  void update() {
    for (int r = 0; r < NR; r++) off[r] += speedOf(r);
    if (winT > 0) {
      if (--winT == 0) { level++; for (auto& h : homes) h = false; resetChick(); }
      return;
    }
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        resetChick();
      }
      return;
    }
    if (--timer <= 0) { die(); return; }
    if (hopT > 0) hopT--;

    // Hopsen (Taste halten = wiederholen)
    int dr = 0, dc = 0;
    if (rUp.tick(cur.up, 200, 160)) dr = -1;
    else if (rDown.tick(cur.down, 200, 160)) dr = 1;
    else if (rLeft.tick(cur.left, 200, 160)) dc = -1;
    else if (rRight.tick(cur.right, 200, 160)) dc = 1;
    if (dr || dc) {
      hopT = 4;
      lastDir = dc;
      if (dr) {
        hr = constrain(hr + dr, 0, NR - 1);
        int k = LANES[hr].kind;
        if (k != LOG && k != TURTLE) hx = roundf(hx / T) * T;   // an Spalten ausrichten
        if (hr < bestRow) { bestRow = hr; score += 10; }
        if (hr == 0) { landHome(); return; }
      } else {
        hx = constrain(hx + dc * T, 0.0f, (float)(cols - 1) * T);
      }
    }

    // Fluss: tragen lassen oder ertrinken
    int k = LANES[hr].kind;
    float c = hx + T / 2.0f;
    if (k == LOG || k == TURTLE) {
      bool safe = false;
      for (int i = 0; i < LANES[hr].count; i++) {
        float x = objX(hr, i);
        if (c >= x + 2 && c <= x + LANES[hr].len * T - 2 && !submerged(hr, i)) { safe = true; break; }
      }
      if (!safe && hopT == 0) { die(); return; }
      if (safe) hx += speedOf(hr);
      if (hx < -T / 2 || hx > SCR_W - T / 2) { die(); return; }
    }
    // Strasse: Fahrzeuge
    if (k == CAR || k == TRUCK) {
      for (int i = 0; i < LANES[hr].count; i++) {
        float x = objX(hr, i);
        if (hitBox(hx + 3, 0, T - 6, 1, x + 1, 0, LANES[hr].len * T - 2, 1)) { die(); return; }
      }
    }
  }

  void drawCar(int x, int y, int len, uint16_t col, bool right, bool truck) {
    int w = len * T - 2, h = T - 4;
    if (truck) {
      int cab = T - 2;
      int cx = right ? x + w - cab : x;
      cRR(x, y + 2, w, h, 2, C_GRAY);
      cRR(cx, y + 2, cab, h, 2, C_RED);
      cR(right ? cx + cab - 5 : cx + 2, y + 4, 3, h - 4, C_CYAN);
    } else {
      cRR(x + 1, y + 2, w - 1, h, 3, col);
      cR(x + w / 2 - 3, y + 4, 6, h - 4, C_BG);
      cR(right ? x + w - 3 : x + 1, y + 3, 2, 3, C_YELLOW);
    }
  }

  void render() {
    // Hintergrund der Reihen
    cR(0, rowY(0), SCR_W, T, C_DGREEN);
    for (int k = 0; k < 5; k++) {
      int hx0 = (int)(homeX(k) - T * 0.8f);
      cR(hx0, rowY(0) + 2, (int)(T * 1.6f), T - 2, RGB(10, 40, 90));
      if (homes[k]) cSpr(CHICK, 8, 8, (int)homeX(k) - T / 2 + (T - 16) / 2, rowY(0) + (T - 16) / 2, C_YELLOW, 2);
    }
    cR(0, rowY(1), SCR_W, 5 * T, RGB(20, 50, 140));
    cR(0, rowY(6), SCR_W, T, C_PURPLE);
    cR(0, rowY(7), SCR_W, 5 * T, RGB(35, 35, 45));
    for (int r = 8; r <= 11; r++)
      for (int x = 4; x < SCR_W; x += 24) cH(x, rowY(r), 10, C_GRAY);
    cR(0, rowY(12), SCR_W, T, C_PURPLE);

    // Objekte
    for (int r = 1; r < NR; r++) {
      const LaneDef& L = LANES[r];
      if (L.kind == NONE) continue;
      for (int i = 0; i < L.count; i++) {
        int x = (int)objX(r, i), y = rowY(r);
        if (x > SCR_W || x + L.len * T < 0) continue;
        if (L.kind == LOG) {
          cRR(x, y + 2, L.len * T, T - 4, 4, RGB(130, 80, 30));
          cH(x + 4, y + T / 2, L.len * T - 8, RGB(90, 50, 20));
        } else if (L.kind == TURTLE) {
          if (submerged(r, i)) continue;
          bool sink = sinking(r, i) && (tick / 4) % 2;
          for (int t = 0; t < L.len; t++) {
            int cx = x + t * T + T / 2;
            cC(cx, y + T / 2, T / 2 - 2, sink ? C_DGREEN : C_GREEN);
            cC(cx, y + T / 2, T / 4, sink ? C_DIM : C_DGREEN);
          }
        } else {
          drawCar(x, y, L.len, L.col, L.speed > 0, L.kind == TRUCK);
        }
      }
    }

    // Kueken
    int s = T >= 16 ? 2 : 1;
    int cw = 8 * s;
    int px = (int)hx + (T - cw) / 2, py = rowY(hr) + (T - cw) / 2 - (hopT > 0 ? 3 : 0);
    if (deadT > 0) {
      cC(px + cw / 2, py + cw / 2, (40 - deadT) / 3 + 2, (deadT / 3) % 2 ? C_RED : C_WHITE);
    } else if (winT == 0) {
      cSpr(CHICK, 8, 8, px, py, C_YELLOW, s, lastDir < 0);
      cR(lastDir < 0 ? px : px + cw - s * 2, py + 3 * s, s * 2, s, C_ORANGE);
    }

    // Zeitleiste und HUD
    int by = rowY(NR) + 3;
    int bw = (SCR_W - 60) * timer / (30 * 30);
    cR(54, by, bw, 5, timer < 300 ? C_RED : C_GREEN);
    cTxt("ZEIT", 6, by - 1, 1, C_GRAY, textdatum_t::top_left);
    char lv[12];
    snprintf(lv, sizeof(lv), "LEVEL %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives - 1 && i < 6; i++) cSpr(CHICK, 8, 8, SCR_W - 70 - i * 10, 4, C_YELLOW, 1);
    if (winT > 0) cTxt("ALLE NESTER VOLL!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
