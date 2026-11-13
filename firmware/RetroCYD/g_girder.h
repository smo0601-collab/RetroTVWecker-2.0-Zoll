// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  GIRDER  -  Ein Bauarbeiter klettert ueber Stahltraeger nach oben, waehrend
//             ein Abriss-Roboter Faesser herunterrollen laesst
// ============================================================================
//  links/rechts laufen, hoch/runter an Leitern klettern, A = springen
//  Ueber ein Fass springen = 100 Punkte. Oben das Funksignal erreichen!
// ============================================================================
#pragma once

namespace Girder {
  const uint16_t WORK0[14] = {0x0078, 0x00FC, 0x00B4, 0x00FC, 0x0078, 0x00FC, 0x01FE, 0x02FD, 0x02FD, 0x00FC, 0x00CC, 0x00CC, 0x00CC, 0x01CE};
  const uint16_t WORK1[14] = {0x0078, 0x00FC, 0x00B4, 0x00FC, 0x0078, 0x00FC, 0x01FE, 0x02FD, 0x02FD, 0x00FC, 0x00CC, 0x004C, 0x00C4, 0x01CE};
  const uint16_t WORKC[14] = {0x0078, 0x00FC, 0x00FC, 0x00FC, 0x0078, 0x02FD, 0x03FE, 0x00FC, 0x00FC, 0x00FC, 0x00CC, 0x00CC, 0x00CC, 0x00CC};
  const uint16_t BOSS[16] = {0x1FF8, 0x3FFC, 0x33CC, 0x33CC, 0x3FFC, 0x1FF8, 0x0A50, 0x7FFE, 0xFFFF, 0xDFFB, 0xDC3B, 0xDFFB, 0x5FFA, 0x1E78, 0x1E78, 0x3E7C};
  const uint16_t BOSSEYE[16] = {0, 0, 0x0C30, 0x0C30, 0, 0, 0, 0, 0, 0, 0x0180, 0, 0, 0, 0, 0};

  const int NL = 6;               // Traeger-Ebenen 0 (unten) bis 5 (oben)
  const int GAP = 28, THICK = 6;
  struct Ladder { float fx; int8_t lo; };
  const Ladder LADDERS[] = {
    {0.78f, 0}, {0.45f, 0}, {0.22f, 1}, {0.60f, 1}, {0.70f, 2}, {0.38f, 2},
    {0.28f, 3}, {0.55f, 3}, {0.76f, 4}, {0.48f, 4},
  };
  const int NLAD = sizeof(LADDERS) / sizeof(LADDERS[0]);

  enum { WALK, CLIMB, JUMP };
  enum { ROLL, FALL, DOWN, OFF };
  struct Barrel { float x, y, vy; int lv, dir; uint8_t st; bool scored; int lad; };
  const int MAXB = 10;
  Barrel barrels[MAXB];

  float px, py, vx, vy;
  int pst, plv, face, walkAnim, ladIdx;
  int lives, level, deadT, winT, throwT, throwAnim, bonus, bonusT;
  uint32_t score;
  int ps;

  int platY(int L) { return SCR_H - 12 - L * ps; }
  bool gapRight(int L) { return L % 2 == 1; }
  int platLeft(int L) { return (L == 0 || gapRight(L)) ? 0 : GAP; }
  int platRight(int L) { return (L == 0 || !gapRight(L)) ? SCR_W : SCR_W - GAP; }
  int rollDir(int L) { return L == 0 ? -1 : gapRight(L) ? 1 : -1; }
  int ladX(int i) { return (int)(LADDERS[i].fx * SCR_W); }
  int goalX() { return platRight(NL - 1) - 18; }

  void resetPlayer() {
    px = 30; plv = 0; py = platY(0); vx = vy = 0; pst = WALK; face = 1;
    for (auto& b : barrels) b.st = OFF;
    throwT = 60; bonus = 5000; bonusT = 0;
  }

  void start() {
    ps = (SCR_H - 60) / NL;
    lives = 3; level = 0; score = 0; deadT = 0; winT = 0; throwAnim = 0;
    resetPlayer();
  }

  void die() {
    if (deadT) return;
    deadT = 60; lives--;
    Snd::fx(Snd::DIE);
    rumble(500, 0xC0);
  }

  void spawnBarrel() {
    for (auto& b : barrels) {
      if (b.st != OFF) continue;
      b = {40, (float)platY(NL - 1), 0, NL - 1, 1, ROLL, false, -1};
      throwAnim = 15;
      return;
    }
  }

  int ladderAt(float x, int lo) {
    for (int i = 0; i < NLAD; i++)
      if (LADDERS[i].lo == lo && fabsf(x - ladX(i)) < 6) return i;
    return -1;
  }

  void updateBarrels() {
    float sp = 1.6f + min(level, 6) * 0.2f;
    for (auto& b : barrels) {
      switch (b.st) {
        case OFF: continue;
        case ROLL: {
          b.x += b.dir * sp;
          // manchmal eine Leiter hinunter
          if (b.lv > 0) {
            int li = ladderAt(b.x, b.lv - 1);
            if (li >= 0 && li != b.lad) {
              b.lad = li;
              if (irand(0, 99) < 25 + level * 5) { b.st = DOWN; b.x = ladX(li); }
            }
          }
          if (b.dir > 0 && b.x > platRight(b.lv) + 2) { b.st = FALL; b.vy = 0; }
          if (b.dir < 0 && b.x < platLeft(b.lv) - 2) {
            if (b.lv == 0) b.st = OFF; else { b.st = FALL; b.vy = 0; }
          }
          break;
        }
        case FALL:
          b.vy += 0.4f; b.y += b.vy;
          if (b.y >= platY(b.lv - 1)) {
            b.lv--; b.y = platY(b.lv); b.st = ROLL; b.dir = rollDir(b.lv); b.lad = -1;
            if (deadT == 0) rumble(30, 0x30);
          }
          break;
        case DOWN:
          b.y += 1.8f;
          if (b.y >= platY(b.lv - 1)) { b.lv--; b.y = platY(b.lv); b.st = ROLL; b.dir = rollDir(b.lv); }
          break;
      }
      if (b.st == OFF) continue;
      // Kollision mit dem Arbeiter
      float by = b.y - 5;
      if (deadT == 0 && fabsf(b.x - px) < 9 && fabsf(by - (py - 7)) < 10) { die(); return; }
      // Drueberspringen
      if (pst == JUMP && !b.scored && fabsf(b.x - px) < 7 && by > py && by - py < 30) {
        b.scored = true; score += 100; bonusT = 30; Snd::fx(Snd::COIN);
      }
    }
  }

  void update() {
    if (throwAnim > 0) throwAnim--;
    if (bonusT > 0) bonusT--;
    if (winT > 0) {
      if (--winT == 0) { level++; resetPlayer(); }
      return;
    }
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        resetPlayer();
      }
      return;
    }
    if (tick % 60 == 0 && bonus > 0) { bonus -= 100; if (bonus <= 0) { die(); return; } }

    // Faesser werfen
    if (--throwT <= 0) {
      spawnBarrel();
      throwT = max(45, 130 - level * 12) + irand(0, 50);
    }

    // Arbeiter
    if (pst == WALK) {
      float dx = 0;
      if (cur.left) dx = -1.8f;
      if (cur.right) dx = 1.8f;
      if (dx != 0) { face = dx > 0 ? 1 : -1; walkAnim++; }
      px = constrain(px + dx, (float)platLeft(plv) + 6, (float)platRight(plv) - 6);
      if (cur.up) {
        int li = ladderAt(px, plv);
        if (li >= 0) { pst = CLIMB; ladIdx = li; px = ladX(li); }
      } else if (cur.down && plv > 0) {
        int li = ladderAt(px, plv - 1);
        if (li >= 0) { pst = CLIMB; ladIdx = li; px = ladX(li); plv--; py = platY(plv + 1) + 1; }
      }
      if (E.a && pst == WALK) { pst = JUMP; vy = -4.4f; vx = dx; Snd::fx(Snd::JUMP); rumble(20, 0x20); }
    } else if (pst == CLIMB) {
      int lo = LADDERS[ladIdx].lo;
      if (cur.up) { py -= 1.4f; walkAnim++; }
      if (cur.down) { py += 1.4f; walkAnim++; }
      if (py <= platY(lo + 1)) { py = platY(lo + 1); plv = lo + 1; pst = WALK; }
      if (py >= platY(lo)) { py = platY(lo); plv = lo; pst = WALK; }
    } else {   // JUMP
      vy += 0.36f;
      py += vy;
      px = constrain(px + vx, (float)platLeft(plv) + 6, (float)platRight(plv) - 6);
      if (vy > 0 && py >= platY(plv)) { py = platY(plv); pst = WALK; }
    }

    // Ziel erreicht?
    if (plv == NL - 1 && pst == WALK && px > goalX() - 10) {
      score += bonus;
      winT = 70;
      Snd::fx(Snd::WIN);
      rumble(300, 0x80);
      return;
    }
    updateBarrels();
  }

  void drawGirder(int L) {
    int x0 = platLeft(L), x1 = platRight(L), y = platY(L);
    uint16_t col = RGB(220, 60, 60);
    cH(x0, y, x1 - x0, col);
    cH(x0, y + THICK - 1, x1 - x0, col);
    for (int x = x0; x < x1 - 4; x += 8) {
      cL(x, y, x + 4, y + THICK - 1, col);
      cL(x + 4, y + THICK - 1, x + 8, y, col);
    }
  }

  void render() {
    for (int i = 0; i < NLAD; i++) {
      int x = ladX(i), yTop = platY(LADDERS[i].lo + 1) + THICK, yBot = platY(LADDERS[i].lo);
      cV(x - 5, yTop, yBot - yTop, C_CYAN);
      cV(x + 5, yTop, yBot - yTop, C_CYAN);
      for (int y = yTop + 3; y < yBot; y += 5) cH(x - 5, y, 11, C_CYAN);
    }
    for (int L = 0; L < NL; L++) drawGirder(L);

    // Boss und Ziel
    int bx = 12, by = platY(NL - 1) - 32;
    cSpr(BOSS, 16, 16, bx, by, RGB(150, 150, 170), 2);
    cSpr(BOSSEYE, 16, 16, bx, by, throwAnim > 0 ? C_RED : C_YELLOW, 2);
    int gx = goalX(), gy = platY(NL - 1);
    cV(gx, gy - 22, 22, C_GRAY);
    int r = 3 + (tick / 4) % 6;
    cCO(gx, gy - 24, r, C_YELLOW);
    cCO(gx, gy - 24, r + 4, C_ORANGE);
    // Faessstapel neben dem Boss
    cC(bx + 40, platY(NL - 1) - 6, 5, RGB(160, 100, 40));

    for (auto& b : barrels) {
      if (b.st == OFF) continue;
      int x = (int)b.x, y = (int)b.y - 6;
      cC(x, y, 5, RGB(170, 105, 40));
      float a = b.x * 0.3f * (b.st == ROLL ? 1 : 0);
      cL(x - (int)(cosf(a) * 4), y - (int)(sinf(a) * 4), x + (int)(cosf(a) * 4), y + (int)(sinf(a) * 4), RGB(90, 50, 20));
    }

    if (deadT == 0 || (deadT / 4) % 2) {
      const uint16_t* spr = pst == CLIMB ? WORKC : ((walkAnim / 5) % 2 ? WORK1 : WORK0);
      int x = (int)px - 5, y = (int)py - 14;
      cSpr(spr, 10, 14, x, y, C_BLUE, 1, face < 0);
      cR(x + 3, y, 4, 2, C_YELLOW);     // Bauhelm
      cR(x + 3, y + 2, 4, 2, RGB(255, 200, 160));
    }
    if (bonusT > 0) cTxt("100", (int)px, (int)py - 26, 1, C_WHITE);

    char lv[16];
    snprintf(lv, sizeof(lv), "BONUS %d", bonus);
    cHud(score, lv);
    for (int i = 0; i < lives - 1 && i < 5; i++) cSpr(WORK0, 10, 14, 6 + i * 12, 16, C_BLUE, 1);
    if (winT > 0) cTxt("GESCHAFFT!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
