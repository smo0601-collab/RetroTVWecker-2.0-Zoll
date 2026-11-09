// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  MAZE BOT  -  Ein Roboter sammelt Energiepunkte im Labyrinth, verfolgt von
//               vier Funken. Batterien machen die Funken kurz harmlos.
// ============================================================================
//  Steuerkreuz/Stick: Richtung (wird an der naechsten Kreuzung genommen)
// ============================================================================
#pragma once

namespace Maze {
  const uint16_t BOT0[8] = {0x0018, 0x0010, 0x007E, 0x005A, 0x007E, 0x003C, 0x005A, 0x0042};
  const uint16_t BOT1[8] = {0x0018, 0x0008, 0x007E, 0x005A, 0x007E, 0x003C, 0x0024, 0x0042};

  const int MW = 19, MH = 21, U = 8;     // Labyrinth 19x21 Felder, 8 Einheiten pro Feld
  const char* const MAP[MH] = {
    "###################",
    "#o...............o#",
    "#.##.##.###.##.##.#",
    "#.................#",
    "#.#..#.#####.#..#.#",
    "#.#.##...#...##.#.#",
    "#...#..#.#.#..#...#",
    "#...#.##...##.#...#",
    "#.#.#..##-##..#.#.#",
    "....#..#   #..#....",
    "#.#.#..#####..#.#.#",
    "#.#...#.....#...#.#",
    "#.###.#.###.#.###.#",
    "#.................#",
    "#.##.#.##.##.#.##.#",
    "#o.#...........#.o#",
    "##.#.#.#####.#.#.##",
    "#....#...#...#....#",
    "#.######.#.######.#",
    "#.................#",
    "###################",
  };
  const int DX[4] = {0, 1, 0, -1}, DY[4] = {-1, 0, 1, 0};
  const uint16_t FOECOL[4] = {C_RED, C_PINK, C_ORANGE, C_GREEN};
  const int CORNER[4][2] = {{17, 1}, {1, 1}, {17, 19}, {1, 19}};

  enum { HOUSE, EXIT, ROAM, EYES };
  struct Mover { int x, y, dir; float acc; };
  struct Foe { Mover m; uint8_t st; int wait; bool scared; };

  uint8_t dots[MH][MW];
  int dotsLeft, T, ox, oy;
  Mover pl;
  int want;
  Foe foes[4];
  int lives, level, deadT, winT, scaredT, chain, modeT, startT;
  bool chase;
  uint32_t score;

  bool wall(int tx, int ty, bool door) {
    if (ty < 0 || ty >= MH) return true;
    tx = (tx + MW) % MW;
    char c = MAP[ty][tx];
    if (c == '#') return true;
    if (c == '-') return !door;
    if (c == ' ') return !door;   // Inneres des Hauses
    return false;
  }

  bool atCenter(const Mover& m) { return m.x % U == 0 && m.y % U == 0; }
  bool canGo(const Mover& m, int d, bool door) {
    return !wall(m.x / U + DX[d], m.y / U + DY[d], door);
  }

  // eine Einheit in Laufrichtung bewegen (mit Tunnel)
  void stepOne(Mover& m) {
    m.x += DX[m.dir];
    m.y += DY[m.dir];
    if (m.x < 0) m.x += MW * U;
    if (m.x >= MW * U) m.x -= MW * U;
  }

  void resetPositions() {
    pl = {9 * U, 15 * U, 3, 0};
    want = 3;
    for (int i = 0; i < 4; i++) {
      Foe& f = foes[i];
      if (i == 0) { f.m = {9 * U, 7 * U, 3, 0}; f.st = ROAM; f.wait = 0; }
      else { f.m = {(7 + i) * U, 9 * U, 0, 0}; f.st = HOUSE; f.wait = 60 + i * 90; }
      f.scared = false;
    }
    scaredT = 0; modeT = 0; chase = false; startT = 45;
  }

  void resetDots() {
    dotsLeft = 0;
    for (int y = 0; y < MH; y++)
      for (int x = 0; x < MW; x++) {
        char c = MAP[y][x];
        dots[y][x] = c == '.' ? 1 : c == 'o' ? 2 : 0;
        if (dots[y][x]) dotsLeft++;
      }
  }

  void start() {
    T = min(SCR_W / MW, (SCR_H - 20) / MH);
    ox = (SCR_W - MW * T) / 2;
    oy = 16 + (SCR_H - 16 - MH * T) / 2;
    lives = 3; level = 0; score = 0; deadT = 0; winT = 0;
    resetDots();
    resetPositions();
  }

  void target(int i, int& tx, int& ty) {
    int px = pl.x / U, py = pl.y / U;
    if (!chase) { tx = CORNER[i][0]; ty = CORNER[i][1]; return; }
    switch (i) {
      case 0: tx = px; ty = py; break;
      case 1: tx = px + DX[pl.dir] * 4; ty = py + DY[pl.dir] * 4; break;
      case 2: tx = px + ((tick / 60) % 2 ? 3 : -3); ty = py + ((tick / 90) % 2 ? 3 : -3); break;
      default: {
        int fx = foes[i].m.x / U, fy = foes[i].m.y / U;
        if ((fx - px) * (fx - px) + (fy - py) * (fy - py) > 64) { tx = px; ty = py; }
        else { tx = CORNER[i][0]; ty = CORNER[i][1]; }
      }
    }
  }

  void chooseDir(Foe& f, int tx, int ty, bool random, bool door) {
    int best = -1; long bestD = 0x7FFFFFFF;
    int start = random ? irand(0, 3) : 0;
    for (int k = 0; k < 4; k++) {
      int d = (start + k) % 4;
      if (d == (f.m.dir + 2) % 4) continue;
      if (!canGo(f.m, d, door)) continue;
      if (random) { best = d; break; }
      int nx = f.m.x / U + DX[d], ny = f.m.y / U + DY[d];
      long dd = (long)(nx - tx) * (nx - tx) + (long)(ny - ty) * (ny - ty);
      if (dd < bestD) { bestD = dd; best = d; }
    }
    if (best < 0) best = (f.m.dir + 2) % 4;   // Sackgasse: umdrehen
    f.m.dir = best;
  }

  void moveFoe(Foe& f, int i) {
    float sp;
    if (f.st == EYES) sp = 2.5f;
    else if (f.scared) sp = 0.55f;
    else if (f.m.y / U == 9 && (f.m.x / U < 3 || f.m.x / U > 15)) sp = 0.5f;   // Tunnel bremst
    else sp = 0.9f + min(level, 6) * 0.06f;
    f.m.acc += sp;
    while (f.m.acc >= 1) {
      f.m.acc -= 1;
      if (f.st == HOUSE) {             // im Haus auf und ab wippen
        f.m.dir = (f.m.y <= 9 * U - 3) ? 2 : (f.m.y >= 9 * U + 3) ? 0 : f.m.dir;
        f.m.y += DY[f.m.dir];
        continue;
      }
      if (f.st == EXIT) {              // zur Tuer und nach oben hinaus
        if (f.m.x != 9 * U) { f.m.x += f.m.x < 9 * U ? 1 : -1; continue; }
        if (f.m.y > 7 * U) { f.m.y -= 1; continue; }
        f.st = ROAM; f.m.dir = irand(0, 1) ? 1 : 3;
        continue;
      }
      if (atCenter(f.m)) {
        if (f.st == EYES && f.m.x == 9 * U && f.m.y == 7 * U) {
          f.st = EXIT; f.m.y = 8 * U; f.scared = false;   // durch die Tuer zurueck
          continue;
        }
        int tx, ty;
        if (f.st == EYES) { tx = 9; ty = 7; } else target(i, tx, ty);
        chooseDir(f, tx, ty, f.scared && f.st != EYES, false);
      }
      stepOne(f.m);
    }
  }

  void loseLife() {
    deadT = 50;
    lives--;
    rumble(500, 0xC0);
  }

  void update() {
    if (winT > 0) {
      if (--winT == 0) { level++; resetDots(); resetPositions(); }
      return;
    }
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        resetPositions();
      }
      return;
    }
    if (startT > 0) { startT--; return; }

    // Modus: Streifen / Jagen
    if (scaredT > 0) {
      if (--scaredT == 0) for (auto& f : foes) f.scared = false;
    } else {
      modeT++;
      int len = chase ? 20 * 30 : 7 * 30;
      if (modeT >= len) {
        modeT = 0; chase = !chase;
        for (auto& f : foes) if (f.st == ROAM) f.m.dir = (f.m.dir + 2) % 4;
      }
    }

    // Spieler
    if (cur.up) want = 0;
    else if (cur.right) want = 1;
    else if (cur.down) want = 2;
    else if (cur.left) want = 3;
    if (want == (pl.dir + 2) % 4) pl.dir = want;   // umdrehen geht sofort
    pl.acc += 1.0f + min(level, 6) * 0.05f;
    while (pl.acc >= 1) {
      pl.acc -= 1;
      if (atCenter(pl)) {
        if (canGo(pl, want, false)) pl.dir = want;
        if (!canGo(pl, pl.dir, false)) { pl.acc = 0; break; }
        int tx = pl.x / U, ty = pl.y / U;
        if (dots[ty][tx]) {
          if (dots[ty][tx] == 2) {
            score += 50;
            scaredT = max(60, 210 - level * 30);
            chain = 0;
            for (auto& f : foes) if (f.st == ROAM) { f.scared = true; f.m.dir = (f.m.dir + 2) % 4; }
            rumble(120, 0x60);
          } else {
            score += 10;
          }
          dots[ty][tx] = 0;
          if (--dotsLeft == 0) { score += 1000; winT = 60; return; }
        }
      }
      stepOne(pl);
    }

    // Gegner
    for (int i = 0; i < 4; i++) {
      Foe& f = foes[i];
      if (f.st == HOUSE && --f.wait <= 0) f.st = EXIT;
      moveFoe(f, i);
      if (f.st == ROAM && abs(f.m.x - pl.x) < 6 && abs(f.m.y - pl.y) < 6) {
        if (f.scared) {
          chain++;
          score += 100 << min(chain, 4);
          f.st = EYES; f.scared = false;
          rumble(100, 0x80);
        } else {
          loseLife();
          return;
        }
      }
    }
  }

  int sx(int ux) { return ox + ux * T / U; }
  int sy(int uy) { return oy + uy * T / U; }

  void render() {
    uint16_t wallFill = winT > 0 && (winT / 6) % 2 ? C_WHITE : RGB(20, 30, 100);
    uint16_t wallEdge = RGB(60, 110, 255);
    for (int y = 0; y < MH; y++) {
      int py = oy + y * T;
      if (!cVis(py, T)) continue;
      for (int x = 0; x < MW; x++) {
        int px = ox + x * T;
        char c = MAP[y][x];
        if (c == '#') {
          cR(px, py, T, T, wallFill);
          if (y > 0 && MAP[y - 1][x] != '#') cH(px, py, T, wallEdge);
          if (y < MH - 1 && MAP[y + 1][x] != '#') cH(px, py + T - 1, T, wallEdge);
          if (x > 0 && MAP[y][x - 1] != '#') cV(px, py, T, wallEdge);
          if (x < MW - 1 && MAP[y][x + 1] != '#') cV(px + T - 1, py, T, wallEdge);
        } else if (c == '-') {
          cR(px, py + T / 2 - 1, T, 2, C_PINK);
        } else if (dots[y][x] == 1) {
          cR(px + T / 2 - 1, py + T / 2 - 1, 2, 2, C_YELLOW);
        } else if (dots[y][x] == 2 && (tick / 8) % 2) {
          cC(px + T / 2, py + T / 2, T / 3, C_YELLOW);
        }
      }
    }
    // Gegner: Funken mit Zacken
    for (int i = 0; i < 4; i++) {
      Foe& f = foes[i];
      int cx = sx(f.m.x) + T / 2, cy = sy(f.m.y) + T / 2;
      int r = T / 2 - 1;
      if (f.st == EYES) {
        cC(cx - 2, cy, 2, C_WHITE); cC(cx + 2, cy, 2, C_WHITE);
        continue;
      }
      uint16_t col = FOECOL[i];
      if (f.scared) col = (scaredT < 60 && (tick / 5) % 2) ? C_WHITE : C_BLUE;
      float a0 = tick * 0.25f + i;
      for (int k = 0; k < 6; k++) {
        float a = a0 + k * 1.047f;
        cL(cx, cy, cx + (int)(cosf(a) * (r + 2)), cy + (int)(sinf(a) * (r + 2)), col);
      }
      cC(cx, cy, r - 1, col);
      cP(cx - 2, cy - 1, C_BG); cP(cx + 2, cy - 1, C_BG);
    }
    // Roboter
    if (deadT == 0 || (deadT / 4) % 2) {
      int px = sx(pl.x) + (T - 8) / 2, py = sy(pl.y) + (T - 8) / 2;
      bool fr = ((pl.x + pl.y) / 3) % 2;
      cSpr(fr ? BOT1 : BOT0, 8, 8, px, py, deadT ? C_RED : C_CYAN, 1);
    }
    char lv[12];
    snprintf(lv, sizeof(lv), "LEVEL %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives - 1 && i < 5; i++) cSpr(BOT0, 8, 8, SCR_W - 70 - i * 11, 4, C_CYAN, 1);
    if (startT > 0 && deadT == 0) cTxt("LOS!", SCR_W / 2, sy(11 * U) + T / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
