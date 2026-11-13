// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  ALIEN WAVE  -  Eine Formation von Aliens rueckt Reihe fuer Reihe vor
// ============================================================================
//  links/rechts bewegen, A = feuern. Schutzbunker halten Treffer ab.
// ============================================================================
#pragma once

namespace Invaders {
  const uint16_t IA0[8] = {0x0070, 0x01FC, 0x0376, 0x07FF, 0x03FE, 0x0154, 0x028A, 0x0401};
  const uint16_t IA1[8] = {0x0070, 0x01FC, 0x0376, 0x07FF, 0x03FE, 0x0154, 0x0154, 0x0202};
  const uint16_t IB0[8] = {0x0306, 0x018C, 0x03FE, 0x06FB, 0x07FF, 0x0505, 0x058D, 0x0000};
  const uint16_t IB1[8] = {0x018C, 0x018C, 0x03FE, 0x06FB, 0x07FF, 0x0306, 0x0603, 0x0000};
  const uint16_t IC0[8] = {0x0020, 0x0070, 0x00F8, 0x01AC, 0x01FC, 0x00A8, 0x0104, 0x0088};
  const uint16_t IC1[8] = {0x0020, 0x0070, 0x00F8, 0x01AC, 0x01FC, 0x00A8, 0x0088, 0x0104};
  const uint16_t PLAYER[6] = {0x0020, 0x0070, 0x0272, 0x03FE, 0x07FF, 0x068B};
  const uint16_t UFO[6] = {0x07E0, 0x1FF8, 0x6DB6, 0xFFFF, 0x399C, 0x1008};
  const uint16_t BOOM[8] = {0x0451, 0x0252, 0x0104, 0x0603, 0x0104, 0x0252, 0x0451, 0x0000};

  const int COLS = 8, ROWS = 5, AW = 22, AH = 16;        // Alien 11x8 mal 2
  const int SH_N = 4, SH_C = 8, SH_R = 5, SH_PX = 3;     // Bunker
  const uint16_t ROWCOL[ROWS] = {C_PINK, C_CYAN, C_CYAN, C_GREEN, C_GREEN};
  const int ROWPTS[ROWS] = {30, 20, 20, 10, 10};

  bool alive[ROWS][COLS];
  uint8_t shield[SH_N][SH_R][SH_C];
  int cw, ch, fx, fy, dir, moveTimer, frame, aliveCount;
  float px;
  int lives, level, deadTimer, waveWait;
  uint32_t score;
  bool pbOn; float pbx, pby;
  struct Bomb { float x, y; bool on; };
  Bomb bombs[5];
  struct Boom { int x, y, t; };
  Boom booms[6];
  bool ufoOn; float ufoX; int ufoDir, ufoTimer;

  int playerY() { return SCR_H - 20; }
  int shieldY() { return SCR_H - 62; }
  int shieldX(int i) { return (SCR_W * (2 * i + 1)) / (2 * SH_N) - SH_C * SH_PX / 2; }
  int alienX(int c) { return fx + c * cw + (cw - AW) / 2; }
  int alienY(int r) { return fy + r * ch; }

  void addBoom(int x, int y) {
    for (auto& b : booms) if (b.t <= 0) { b = {x, y, 8}; return; }
  }

  void resetShields() {
    for (int s = 0; s < SH_N; s++)
      for (int r = 0; r < SH_R; r++)
        for (int c = 0; c < SH_C; c++) {
          bool hole = (r == 0 && (c == 0 || c == SH_C - 1)) ||
                      (r >= SH_R - 2 && c >= 3 && c <= 4);
          shield[s][r][c] = hole ? 0 : 2;
        }
  }

  void startWave() {
    for (int r = 0; r < ROWS; r++) for (int c = 0; c < COLS; c++) alive[r][c] = true;
    aliveCount = ROWS * COLS;
    cw = min(26, (SCR_W - 24) / COLS);
    ch = 22;
    fx = (SCR_W - COLS * cw) / 2;
    fy = 30 + min(level, 6) * 8;
    dir = 1; moveTimer = 0; frame = 0;
    for (auto& b : bombs) b.on = false;
    pbOn = false;
    ufoOn = false; ufoTimer = irand(400, 700);
    resetShields();
  }

  void start() {
    lives = 3; level = 0; score = 0; deadTimer = 0; waveWait = 0;
    px = SCR_W / 2;
    for (auto& b : booms) b.t = 0;
    startWave();
  }

  bool hitShield(float x, float y, int w, int h) {
    for (int s = 0; s < SH_N; s++) {
      int sx = shieldX(s), sy = shieldY();
      if (!hitBox(x, y, w, h, sx, sy, SH_C * SH_PX, SH_R * SH_PX)) continue;
      for (int r = 0; r < SH_R; r++)
        for (int c = 0; c < SH_C; c++) {
          if (!shield[s][r][c]) continue;
          if (hitBox(x, y, w, h, sx + c * SH_PX, sy + r * SH_PX, SH_PX, SH_PX)) {
            shield[s][r][c]--;
            return true;
          }
        }
    }
    return false;
  }

  void playerHit() {
    lives--;
    addBoom((int)px - 11, playerY() - 4);
    Snd::fx(Snd::DIE);
    rumble(500, 0xC0);
    deadTimer = 60;
    for (auto& b : bombs) b.on = false;
  }

  void stepFormation() {
    // Rand erreicht? -> eine Reihe tiefer und Richtung wechseln
    int minC = COLS, maxC = -1, maxR = -1;
    for (int r = 0; r < ROWS; r++)
      for (int c = 0; c < COLS; c++)
        if (alive[r][c]) { minC = min(minC, c); maxC = max(maxC, c); maxR = max(maxR, r); }
    if (maxR < 0) return;
    int step = 4;
    int leftX = alienX(minC) + dir * step, rightX = alienX(maxC) + AW + dir * step;
    if (leftX < 4 || rightX > SCR_W - 4) {
      fy += 8;
      dir = -dir;
    } else {
      fx += dir * step;
    }
    frame ^= 1;
    static const uint16_t MARCH[4] = {196, 175, 165, 147};   // der bekannte Marschschritt
    static uint8_t ms = 0;
    Snd::pitch(MARCH[ms++ & 3], 0, 70, sfx::DC, Snd::P_AUTO);
    // Aliens fressen sich durch die Bunker
    for (int c = 0; c < COLS; c++)
      for (int r = 0; r < ROWS; r++)
        if (alive[r][c])
          while (hitShield(alienX(c), alienY(r), AW, AH)) {}
    if (alienY(maxR) + AH >= playerY() - 6) { lives = 0; gameOver(score); }
  }

  void update() {
    if (waveWait > 0) { if (--waveWait == 0) { level++; startWave(); } return; }

    // Spieler
    if (deadTimer > 0) {
      if (--deadTimer == 0 && lives <= 0) { gameOver(score); return; }
    } else {
      float sp = 4.0f;
      if (cur.left) px -= sp;
      if (cur.right) px += sp;
      if (abs(cur.ax) > 300 && !cur.left && !cur.right) px += cur.ax / 512.0f * sp;
      px = constrain(px, 14.0f, (float)SCR_W - 14);
      if ((E.a || cur.a) && !pbOn) { pbOn = true; pbx = px; pby = playerY() - 6; Snd::fx(Snd::LASER); }
    }

    // Formation bewegen: je weniger Aliens, desto schneller
    int interval = 1 + aliveCount * 13 / (ROWS * COLS) - min(level, 3) / 2;
    if (interval < 1) interval = 1;
    if (++moveTimer >= interval) { moveTimer = 0; stepFormation(); if (state != ST_GAME) return; }

    // Spielerschuss
    if (pbOn) {
      pby -= 9;
      if (pby < 16) pbOn = false;
      else if (hitShield(pbx - 1, pby, 2, 8)) pbOn = false;
      else if (ufoOn && hitBox(pbx - 1, pby, 2, 8, ufoX, 20, 32, 12)) {
        pbOn = false; ufoOn = false;
        int pts = 50 * irand(1, 6);
        score += pts;
        addBoom((int)ufoX + 5, 20);
        Snd::fx(Snd::COIN);
        rumble(200, 0x80);
      } else {
        for (int r = 0; r < ROWS && pbOn; r++)
          for (int c = 0; c < COLS; c++)
            if (alive[r][c] && hitBox(pbx - 1, pby, 2, 8, alienX(c), alienY(r), AW, AH)) {
              alive[r][c] = false;
              aliveCount--;
              score += ROWPTS[r];
              addBoom(alienX(c), alienY(r));
              Snd::fx(Snd::THUD);
              rumble(40, 0x40);
              pbOn = false;
              break;
            }
      }
    }

    // Aliens werfen Bomben (vom untersten Alien einer zufaelligen Spalte)
    int chance = 22 - min(level * 2, 12);
    if (deadTimer == 0 && irand(0, chance) == 0) {
      int c = irand(0, COLS - 1);
      for (int r = ROWS - 1; r >= 0; r--) {
        if (!alive[r][c]) continue;
        for (auto& b : bombs)
          if (!b.on) { b = {(float)alienX(c) + AW / 2, (float)alienY(r) + AH, true}; break; }
        break;
      }
    }
    float bsp = 3.2f + min(level, 6) * 0.3f;
    for (auto& b : bombs) {
      if (!b.on) continue;
      b.y += bsp;
      if (b.y > SCR_H) b.on = false;
      else if (hitShield(b.x - 1, b.y, 3, 6)) b.on = false;
      else if (pbOn && fabsf(b.x - pbx) < 4 && fabsf(b.y - pby) < 8) { b.on = false; pbOn = false; }
      else if (deadTimer == 0 && hitBox(b.x - 1, b.y, 3, 6, px - 11, playerY() - 6, 22, 12)) {
        b.on = false;
        playerHit();
      }
    }

    // UFO
    if (!ufoOn && aliveCount > 8 && --ufoTimer <= 0) {
      ufoOn = true; Snd::fx(Snd::LOOP, Snd::P_AUTO); ufoDir = irand(0, 1) ? 1 : -1; ufoX = ufoDir > 0 ? -32 : SCR_W;
    }
    if (ufoOn) {
      ufoX += ufoDir * 2.0f;
      if (ufoX < -40 || ufoX > SCR_W + 8) { ufoOn = false; ufoTimer = irand(500, 800); }
    }
    for (auto& b : booms) if (b.t > 0) b.t--;

    if (aliveCount == 0 && waveWait == 0) { waveWait = 45; score += 500; Snd::fx(Snd::WIN); }
  }

  void render() {
    for (int r = 0; r < ROWS; r++) {
      const uint16_t* spr = r == 0 ? (frame ? IC1 : IC0) : r < 3 ? (frame ? IB1 : IB0) : (frame ? IA1 : IA0);
      for (int c = 0; c < COLS; c++)
        if (alive[r][c]) cSpr(spr, 11, 8, alienX(c), alienY(r), ROWCOL[r], 2);
    }
    for (int s = 0; s < SH_N; s++)
      for (int r = 0; r < SH_R; r++)
        for (int c = 0; c < SH_C; c++)
          if (shield[s][r][c])
            cR(shieldX(s) + c * SH_PX, shieldY() + r * SH_PX, SH_PX, SH_PX,
               shield[s][r][c] == 2 ? C_GREEN : C_DGREEN);
    if (ufoOn) cSpr(UFO, 16, 6, (int)ufoX, 20, C_RED, 2);
    if (deadTimer == 0 || (tick / 4) % 2) cSpr(PLAYER, 11, 6, (int)px - 11, playerY() - 6, C_CYAN, 2);
    if (pbOn) cR((int)pbx - 1, (int)pby, 2, 8, C_WHITE);
    for (auto& b : bombs) {
      if (!b.on) continue;
      int o = ((int)b.y / 3) % 2 ? 1 : -1;
      cL((int)b.x, (int)b.y, (int)b.x + o, (int)b.y + 3, C_ORANGE);
      cL((int)b.x + o, (int)b.y + 3, (int)b.x, (int)b.y + 6, C_ORANGE);
    }
    for (auto& b : booms) if (b.t > 0) cSpr(BOOM, 11, 8, b.x, b.y, b.t > 4 ? C_YELLOW : C_ORANGE, 2);
    cH(0, SCR_H - 8, SCR_W, C_DGREEN);
    char lv[12];
    snprintf(lv, sizeof(lv), "WELLE %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives && i < 5; i++) cSpr(PLAYER, 11, 6, 6 + i * 14, SCR_H - 7, C_CYAN, 1);
    if (waveWait > 0) cTxt("WELLE GESCHAFFT!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
