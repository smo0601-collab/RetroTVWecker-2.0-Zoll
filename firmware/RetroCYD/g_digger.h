// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  DIG DEEP  -  Tunnel graben, Gegner mit der Luftpumpe aufblasen bis sie
//               platzen, Felsbrocken auf sie fallen lassen
// ============================================================================
//  Steuerkreuz: graben/laufen, A = Pumpe abschiessen und weiter pumpen
//  (mehrmals A druecken). Felsen fallen, wenn darunter gegraben wird.
// ============================================================================
#pragma once

namespace Digger {
  const uint16_t DIG0[10] = {0x0078, 0x00FC, 0x01B6, 0x01FE, 0x00FC, 0x01FE, 0x02FD, 0x00FC, 0x00CC, 0x01CE};
  const uint16_t DIG1[10] = {0x0078, 0x00FC, 0x01B6, 0x01FE, 0x00FC, 0x01FE, 0x02FD, 0x00FC, 0x00C4, 0x01CC};
  const uint16_t PUFF[10] = {0x0078, 0x01FE, 0x0333, 0x02CD, 0x0333, 0x03FF, 0x03FF, 0x01FE, 0x017A, 0x0084};
  const uint16_t LIZ[10]  = {0x0006, 0x000F, 0x001B, 0x00FE, 0x01FE, 0x03FF, 0x02FE, 0x00B4, 0x0082, 0x0186};

  const int T = 16;                      // Feldgroesse in Pixeln
  const int MAXC = 20, MAXR = 19;
  const int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
  const uint16_t LAYER[4] = {RGB(200, 140, 60), RGB(170, 100, 40), RGB(140, 70, 30), RGB(110, 45, 25)};

  bool dug[MAXR][MAXC];
  int cols, rows, oy;

  struct Ent { int x, y, dir; };          // Pixel, an Felder ausgerichtet
  Ent pl;
  int want, face, walk;

  enum { WALKING, GHOST, PUMPED, POPPED, CRUSHED };
  struct Foe { Ent e; uint8_t type, st; int infl, deflT, ghostT, fireT, popT; bool on; };
  const int MAXF = 8;
  Foe foes[MAXF];

  struct Rock { int x, y; float vy; int st, t; bool on; };   // st: 0 ruht, 1 wackelt, 2 faellt
  const int MAXROCK = 5;
  Rock rocks[MAXROCK];

  int pumpLen, pumpFoe, pumpHold;       // Pumpe: Laenge in Pixeln, getroffener Gegner
  bool pumping;
  int lives, level, deadT, winT, nFoes;
  uint32_t score;

  bool inside(int c, int r) { return c >= 0 && c < cols && r >= 0 && r < rows; }
  bool open(int c, int r) { return inside(c, r) && (r == 0 || dug[r][c]); }
  int layerOf(int r) { return constrain((r - 1) * 4 / max(1, rows - 1), 0, 3); }
  bool aligned(const Ent& e) { return e.x % T == 0 && (e.y - oy) % T == 0; }
  int tc(int x) { return (x + T / 2) / T; }
  int tr(int y) { return (y - oy + T / 2) / T; }
  int rockAt(int c, int r) {
    for (int i = 0; i < MAXROCK; i++)
      if (rocks[i].on && rocks[i].st != 2 && rocks[i].x == c * T && rocks[i].y == oy + r * T) return i;
    return -1;
  }

  void digTunnel(int c, int r, int dir, int len) {
    for (int i = 0; i < len; i++) {
      int cc = c + DX[dir] * i, rr = r + DY[dir] * i;
      if (inside(cc, rr)) dug[rr][cc] = true;
    }
  }

  void setupLevel() {
    memset(dug, 0, sizeof(dug));
    // senkrechter Schacht vom Startpunkt
    int sc = cols / 2;
    digTunnel(sc, 1, 2, rows / 2 - 1);
    nFoes = min(3 + level, MAXF);
    for (int i = 0; i < MAXF; i++) foes[i].on = false;
    for (int i = 0; i < nFoes; i++) {
      Foe& f = foes[i];
      int c, r;
      do { c = irand(1, cols - 4); r = irand(3, rows - 2); } while (abs(c - sc) < 3);
      int dir = irand(0, 1) ? 1 : 2;
      digTunnel(c, r, dir, 3);
      f.e = {c * T, oy + r * T, dir};
      f.type = (i % 3 == 2) ? 1 : 0;
      f.st = WALKING; f.infl = 0; f.deflT = 0; f.ghostT = irand(15, 35); f.fireT = irand(60, 200);
      f.popT = 0; f.on = true;
    }
    for (int i = 0; i < MAXROCK; i++) {
      Rock& k = rocks[i];
      k.on = i < 3 + min(level, 2);
      if (!k.on) continue;
      int c, r;
      int tries = 0;
      do { c = irand(0, cols - 1); r = irand(2, rows - 3); tries++; }
      while ((dug[r][c] || dug[r + 1][c]) && tries < 50);
      k = {c * T, oy + r * T, 0, 0, 0, true};
    }
  }

  void resetPlayer() {
    pl = {(cols / 2) * T, oy, 2};
    want = 2; face = 1; pumpLen = 0; pumpFoe = -1; pumping = false; pumpHold = 0;
  }

  void start() {
    cols = min(MAXC, SCR_W / T);
    oy = 16;
    rows = min(MAXR, (SCR_H - oy) / T);
    lives = 3; level = 0; score = 0; deadT = 0; winT = 0;
    setupLevel();
    resetPlayer();
  }

  void die() {
    if (deadT) return;
    deadT = 60; lives--;
    Snd::fx(Snd::DIE);
    rumble(500, 0xC0);
  }

  void popFoe(Foe& f) {
    f.st = POPPED; f.popT = 20;
    score += 200 + layerOf(tr(f.e.y)) * 100;
    Snd::fx(Snd::SPLASH);
    rumble(120, 0x80);
  }

  // Gegner laufen durch Tunnel; Geister schweben durch die Erde zum Spieler
  void moveFoe(Foe& f) {
    float sp = 1 + (level >= 3);
    if (f.st == GHOST) {
      if (tick % 2) return;
      int dx = pl.x - f.e.x, dy = pl.y - f.e.y;
      if (abs(dx) > abs(dy)) f.e.x += dx > 0 ? 1 : -1; else f.e.y += dy > 0 ? 1 : -1;
      f.ghostT++;
      if (f.ghostT > 90 && aligned(f.e) && open(tc(f.e.x), tr(f.e.y)) && tr(f.e.y) > 0) {
        f.st = WALKING; f.ghostT = irand(15, 35) - min(level, 8);
      }
      return;
    }
    for (int s = 0; s < sp; s++) {
      if (aligned(f.e)) {
        int c = tc(f.e.x), r = tr(f.e.y);
        int opts[4], n = 0;
        for (int d = 0; d < 4; d++)
          if (d != (f.e.dir + 2) % 4 && open(c + DX[d], r + DY[d]) && r + DY[d] > 0) opts[n++] = d;
        if (n == 0) f.e.dir = (f.e.dir + 2) % 4;
        else {
          // bevorzugt in Richtung Spieler
          int best = opts[irand(0, n - 1)];
          if (irand(0, 2)) {
            long bd = 0x7FFFFFFF;
            for (int k = 0; k < n; k++) {
              int nx = (c + DX[opts[k]]) * T, ny = oy + (r + DY[opts[k]]) * T;
              long d = (long)(nx - pl.x) * (nx - pl.x) + (long)(ny - pl.y) * (ny - pl.y);
              if (d < bd) { bd = d; best = opts[k]; }
            }
          }
          f.e.dir = best;
        }
        if (--f.ghostT <= 0) { f.st = GHOST; f.ghostT = 0; return; }
      }
      f.e.x += DX[f.e.dir];
      f.e.y += DY[f.e.dir];
    }
  }

  void updateRocks() {
    for (auto& k : rocks) {
      if (!k.on) continue;
      int c = k.x / T, r = (k.y - oy) / T;
      if (k.st == 0) {
        bool under = r + 1 < rows && dug[r + 1][c];
        bool plUnder = tc(pl.x) == c && tr(pl.y) == r + 1;
        if (under && !plUnder) { k.st = 1; k.t = 30; }
      } else if (k.st == 1) {
        if (--k.t <= 0) { k.st = 2; k.vy = 0; }
      } else {
        k.vy = min(k.vy + 0.35f, 4.0f);
        k.y += (int)k.vy + 1;
        int nr = (k.y - oy) / T + 1;
        for (auto& f : foes)
          if (f.on && f.st != POPPED && f.st != CRUSHED && abs(f.e.x - k.x) < T - 2 && abs(f.e.y - k.y) < T - 2) {
            f.st = CRUSHED; f.popT = 25; score += 1000; rumble(150, 0x90);
          }
        if (deadT == 0 && abs(pl.x - k.x) < T - 4 && abs(pl.y - k.y) < T - 4) die();
        if (nr >= rows || !dug[nr][c]) {
          k.y = oy + (nr - 1) * T;
          k.on = false;           // zerbricht beim Aufschlag
          rumble(80, 0x60);
        }
      }
    }
  }

  void update() {
    if (winT > 0) {
      if (--winT == 0) { level++; setupLevel(); resetPlayer(); }
      return;
    }
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        resetPlayer();
        for (auto& f : foes) if (f.on && (f.st == PUMPED)) { f.st = WALKING; f.infl = 0; }
      }
      return;
    }

    // Pumpe
    if (pumping) {
      if (pumpFoe >= 0) {
        Foe& f = foes[pumpFoe];
        if (E.a) {
          f.infl++; f.deflT = 30; pumpHold = 25;
          Snd::pitch(300 + f.infl * 150, 500 + f.infl * 150, 70);
          rumble(40, 0x40 + f.infl * 0x20);
          if (f.infl >= 4) { popFoe(f); pumping = false; pumpFoe = -1; pumpLen = 0; }
        }
        if (pumping && --pumpHold <= 0) { pumping = false; pumpFoe = -1; pumpLen = 0; }
        if (cur.left || cur.right || cur.up || cur.down) { pumping = false; pumpFoe = -1; pumpLen = 0; }
      } else {
        pumpLen += 6;
        int c = tc(pl.x), r = tr(pl.y);
        int tipX = pl.x + T / 2 + DX[face] * pumpLen, tipY = pl.y + T / 2 + DY[face] * pumpLen;
        int tcx = tipX / T, tcy = (tipY - oy) / T;
        if (pumpLen > 3 * T || !inside(tcx, tcy) || (!open(tcx, tcy) && !(tcx == c && tcy == r))) {
          pumping = false; pumpLen = 0;
        } else {
          for (int i = 0; i < MAXF; i++) {
            Foe& f = foes[i];
            if (!f.on || f.st == POPPED || f.st == CRUSHED || f.st == GHOST) continue;
            if (abs(f.e.x + T / 2 - tipX) < T / 2 + 2 && abs(f.e.y + T / 2 - tipY) < T / 2 + 2) {
              pumpFoe = i; f.st = PUMPED; f.infl = 1; f.deflT = 30; pumpHold = 25;
              break;
            }
          }
        }
      }
    } else if (E.a) {
      pumping = true; pumpLen = 0; pumpFoe = -1;
      Snd::fx(Snd::SHOOT, Snd::P_AUTO);
    }

    // Spieler bewegen (nur ohne Pumpe)
    if (!pumping) {
      if (cur.up) want = 0; else if (cur.right) want = 1; else if (cur.down) want = 2; else if (cur.left) want = 3;
      bool moving = cur.up || cur.right || cur.down || cur.left;
      if (moving) {
        if (want == (pl.dir + 2) % 4) pl.dir = want;
        if (aligned(pl)) pl.dir = want;
        int c = tc(pl.x), r = tr(pl.y);
        int nc = c + DX[pl.dir], nr = r + DY[pl.dir];
        bool blocked = !inside(nc, nr) || (aligned(pl) && rockAt(nc, nr) >= 0);
        if (!blocked || !aligned(pl)) {
          pl.x += DX[pl.dir] * 2;
          pl.y += DY[pl.dir] * 2;
          walk++;
          if (pl.dir == 1) face = 1; else if (pl.dir == 3) face = 3; else face = pl.dir;
          int cc = tc(pl.x), rr = tr(pl.y);
          if (inside(cc, rr) && rr > 0 && !dug[rr][cc]) { dug[rr][cc] = true; if (walk % 4 == 0) score += 1; Snd::fx(Snd::DIG, Snd::P_AUTO); }
        }
      }
    }

    // Gegner
    int alive = 0;
    for (auto& f : foes) {
      if (!f.on) continue;
      if (f.st == POPPED || f.st == CRUSHED) { if (--f.popT <= 0) f.on = false; else alive++; continue; }
      alive++;
      if (f.st == PUMPED) {
        if (--f.deflT <= 0) { f.infl--; f.deflT = 30; if (f.infl <= 0) f.st = WALKING; }
        continue;
      }
      moveFoe(f);
      // Feuerechse speit Feuer
      if (f.type == 1 && f.st == WALKING && --f.fireT <= 0) {
        f.fireT = irand(90, 200);
        int dir = f.e.dir == 3 ? -1 : 1;
        int dx = (pl.x - f.e.x) * dir;
        if (abs(f.e.y - pl.y) < T / 2 && dx > 0 && dx < 3 * T + T / 2) die();
      }
      if (deadT == 0 && abs(f.e.x - pl.x) < T - 4 && abs(f.e.y - pl.y) < T - 4) die();
    }
    updateRocks();
    if (alive == 0 && winT == 0) { winT = 60; score += 1000; Snd::fx(Snd::WIN); }
  }

  void render() {
    cR(0, oy, SCR_W, T, RGB(40, 60, 140));   // Himmel
    for (int r = 1; r < rows; r++) {
      int y = oy + r * T;
      if (!cVis(y, T)) continue;
      uint16_t col = LAYER[layerOf(r)];
      for (int c = 0; c < cols; c++) {
        if (dug[r][c]) continue;
        cR(c * T, y, T, T, col);
        if ((c + r) % 3 == 0) cP(c * T + 4, y + 5, C_BG);
        if ((c * 7 + r) % 5 == 0) cP(c * T + 11, y + 10, C_BG);
      }
    }
    for (auto& k : rocks) {
      if (!k.on) continue;
      int wob = k.st == 1 ? ((tick / 2) % 2 ? 1 : -1) : 0;
      cE(k.x + T / 2 + wob, k.y + T / 2, T / 2 - 1, T / 2 - 2, C_GRAY);
      cE(k.x + T / 2 + wob - 2, k.y + T / 2 - 2, 3, 2, C_WHITE);
    }
    for (auto& f : foes) {
      if (!f.on) continue;
      int x = f.e.x + 3, y = f.e.y + 3;
      if (f.st == POPPED) { cCO(f.e.x + T / 2, f.e.y + T / 2, 25 - f.popT, C_WHITE); continue; }
      if (f.st == CRUSHED) { cR(f.e.x + 2, f.e.y + T - 5, T - 4, 4, f.type ? C_GREEN : C_RED); continue; }
      if (f.st == GHOST) {
        cC(f.e.x + 5, f.e.y + 8, 2, C_WHITE); cC(f.e.x + 11, f.e.y + 8, 2, C_WHITE);
        continue;
      }
      if (f.st == PUMPED) {
        int rr = 6 + f.infl * 3;
        cC(f.e.x + T / 2, f.e.y + T / 2, rr, f.type ? C_GREEN : C_RED);
        cC(f.e.x + T / 2 - 3, f.e.y + T / 2 - 2, 2, C_WHITE);
        continue;
      }
      bool flip = f.e.dir == 3;
      if (f.type == 0) cSpr(PUFF, 10, 10, x, y, C_RED, 1, flip);
      else cSpr(LIZ, 10, 10, x, y, C_GREEN, 1, flip);
      if (f.type == 1 && f.fireT < 15) {
        int dir = flip ? -1 : 1;
        for (int k = 0; k < 3; k++)
          cC(f.e.x + T / 2 + dir * (T + k * 10), f.e.y + T / 2, 5 - k, k ? C_ORANGE : C_YELLOW);
      }
    }
    // Pumpe
    if (pumping) {
      int x0 = pl.x + T / 2, y0 = pl.y + T / 2;
      int len = pumpFoe >= 0 ? max(abs(foes[pumpFoe].e.x - pl.x), abs(foes[pumpFoe].e.y - pl.y)) : pumpLen;
      cL(x0, y0, x0 + DX[face] * len, y0 + DY[face] * len, C_WHITE);
    }
    if (deadT == 0 || (deadT / 4) % 2) {
      cSpr((walk / 4) % 2 ? DIG1 : DIG0, 10, 10, pl.x + 3, pl.y + 3, C_WHITE, 1, face == 3);
      cR(pl.x + 6, pl.y + 3, 4, 2, C_BLUE);
    }
    char lv[12];
    snprintf(lv, sizeof(lv), "RUNDE %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives - 1 && i < 4; i++) cSpr(DIG0, 10, 10, SCR_W - 72 - i * 12, 4, C_WHITE, 1);
    if (winT > 0) cTxt("RUNDE GESCHAFFT!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
