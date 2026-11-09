// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  STAR SWARM  -  Insektenartige Raumschiffe fliegen ein, formieren sich und
//                 stuerzen sich im Sturzflug auf den Spieler
// ============================================================================
//  links/rechts bewegen, A = feuern (2 Schuesse gleichzeitig)
// ============================================================================
#pragma once

namespace Swarm {
  const uint16_t SHIP[9] = {0x0020, 0x0020, 0x0070, 0x0050, 0x0272, 0x02FA, 0x07FF, 0x06DB, 0x0421};
  const uint16_t CM0[8] = {0x0088, 0x0050, 0x01FC, 0x0376, 0x07FF, 0x05FD, 0x0505, 0x00D8};
  const uint16_t CM1[8] = {0x0088, 0x0050, 0x01FC, 0x0376, 0x07FF, 0x01FC, 0x028A, 0x0489};
  const uint16_t ST0[8] = {0x0401, 0x0272, 0x01FC, 0x03AE, 0x01FC, 0x00F8, 0x0104, 0x0202};
  const uint16_t ST1[8] = {0x0000, 0x0673, 0x01FC, 0x03AE, 0x01FC, 0x00F8, 0x0088, 0x0088};
  const uint16_t DR0[8] = {0x0070, 0x00F8, 0x05AD, 0x07FF, 0x05FD, 0x00A8, 0x0104, 0x0000};
  const uint16_t DR1[8] = {0x0070, 0x00F8, 0x01AC, 0x03FE, 0x05FD, 0x04A9, 0x0088, 0x0000};

  enum { DEAD, WAIT, ENTER, FORM, DIVE, BACK };
  struct Foe { uint8_t st, row, type, hp; int8_t col; float x, y, t, sx, sy, cx, cy, ph; int delay, shots; };
  struct Shot { float x, y, vx, vy; bool on; };
  struct Star { float x, y, v; };

  const int ROWN[4] = {4, 6, 8, 8};
  const int MAXF = 26;
  GArr<Foe, MAXF> foes;
  Shot mine[2], theirs[6];
  GArr<Star, 36> stars;
  struct Boom { float x, y; int t; };
  Boom booms[8];

  float px, sway;
  int lives, level, deadTimer, diveTimer, waveWait, nFoes;
  uint32_t score, nextLife;

  int spacing() { return min(24, (SCR_W - 20) / 8); }
  float slotX(int row, int col) { return SCR_W / 2 + (col - (ROWN[row] - 1) / 2.0f) * spacing() + sway; }
  float slotY(int row) { return 34 + row * 20; }
  int playerY() { return SCR_H - 26; }

  void boom(float x, float y) { for (auto& b : booms) if (b.t <= 0) { b = {x, y, 12}; return; } }

  void startWave() {
    int i = 0;
    for (int r = 0; r < 4; r++)
      for (int c = 0; c < ROWN[r]; c++, i++) {
        Foe& f = foes[i];
        f.st = WAIT; f.row = r; f.col = c;
        f.type = r == 0 ? 0 : r == 1 ? 1 : 2;
        f.hp = f.type == 0 ? 2 : 1;
        int group = (r * 8 + c) / 6;
        f.delay = 20 + group * 45 + (i % 6) * 7;
        bool left = group % 2 == 0;
        f.sx = left ? -20 : SCR_W + 20;
        f.sy = SCR_H * 0.18f;
        f.cx = SCR_W / 2 + (left ? 1 : -1) * SCR_W * 0.35f;
        f.cy = SCR_H * 0.8f;
        f.t = 0; f.x = f.sx; f.y = f.sy; f.shots = 0;
      }
    nFoes = i;
    diveTimer = 120;
    for (auto& s : theirs) s.on = false;
  }

  void start() {
    foes.alloc();
    stars.alloc();
    px = SCR_W / 2; lives = 3; level = 0; score = 0; nextLife = 20000;
    deadTimer = 0; waveWait = 0; sway = 0;
    for (auto& s : mine) s.on = false;
    for (auto& b : booms) b.t = 0;
    for (auto& s : stars) s = {frand(0, SCR_W), frand(0, SCR_H), frand(0.5f, 2.5f)};
    startWave();
  }

  void enemyShoot(Foe& f) {
    for (auto& s : theirs) {
      if (s.on) continue;
      float dx = px - f.x;
      s = {f.x, f.y + 8, constrain(dx / 60.0f, -1.5f, 1.5f), 4.0f + min(level, 5) * 0.3f, true};
      return;
    }
  }

  void killPlayer() {
    lives--;
    boom(px, playerY());
    rumble(500, 0xC0);
    deadTimer = 75;
    for (auto& s : theirs) s.on = false;
  }

  void hitFoe(Foe& f) {
    if (--f.hp > 0) { rumble(30, 0x30); return; }
    bool diving = f.st == DIVE || f.st == BACK;
    const int base[3] = {150, 80, 50};
    score += base[f.type] * (diving ? 2 : 1);
    if (score >= nextLife) { lives++; nextLife += 20000; }
    f.st = DEAD;
    boom(f.x, f.y);
    rumble(50, 0x50);
  }

  void update() {
    for (auto& s : stars) { s.y += s.v; if (s.y > SCR_H) { s.y = 0; s.x = frand(0, SCR_W); } }
    for (auto& b : booms) if (b.t > 0) b.t--;
    if (waveWait > 0) { if (--waveWait == 0) { level++; startWave(); } return; }

    sway = sinf(tick * 0.03f) * 14;

    if (deadTimer > 0) {
      if (--deadTimer == 0 && lives <= 0) { gameOver(score); return; }
    } else {
      if (cur.left) px -= 4.2f;
      if (cur.right) px += 4.2f;
      if (abs(cur.ax) > 300 && !cur.left && !cur.right) px += cur.ax / 512.0f * 4.2f;
      px = constrain(px, 12.0f, (float)SCR_W - 12);
      if (E.a || (cur.a && tick % 6 == 0)) {
        for (auto& s : mine) if (!s.on) { s = {px, (float)playerY() - 10, 0, -9, true}; break; }
      }
    }
    for (auto& s : mine) if (s.on) { s.y += s.vy; if (s.y < 14) s.on = false; }

    int alive = 0, inForm = 0;
    for (auto& f : foes) {
      if (f.st == DEAD) continue;
      alive++;
      switch (f.st) {
        case WAIT:
          if (--f.delay <= 0) f.st = ENTER;
          break;
        case ENTER: {
          f.t += 0.017f;
          float ex = slotX(f.row, f.col), ey = slotY(f.row);
          if (f.t >= 1) { f.st = FORM; f.x = ex; f.y = ey; break; }
          float u = 1 - f.t;
          f.x = u * u * f.sx + 2 * u * f.t * f.cx + f.t * f.t * ex;
          f.y = u * u * f.sy + 2 * u * f.t * f.cy + f.t * f.t * ey;
          if (f.t > 0.45f && f.t < 0.47f && irand(0, 8) == 0) enemyShoot(f);
          break;
        }
        case FORM:
          inForm++;
          f.x = slotX(f.row, f.col);
          f.y = slotY(f.row) + sinf(tick * 0.06f + f.col) * 1.5f;
          break;
        case DIVE:
          f.ph += 0.08f;
          f.y += 2.6f + min(level, 8) * 0.18f;
          f.x += (px - f.x) * 0.018f + sinf(f.ph) * 2.2f;
          if (f.shots < 2 && f.y > SCR_H * (0.3f + 0.15f * f.shots) && irand(0, 3) == 0) {
            enemyShoot(f); f.shots++;
          }
          if (f.y > SCR_H + 12) { f.y = -12; f.st = BACK; }
          break;
        case BACK: {
          float ex = slotX(f.row, f.col), ey = slotY(f.row);
          float dx = ex - f.x, dy = ey - f.y, d = sqrtf(dx * dx + dy * dy);
          if (d < 3) { f.st = FORM; break; }
          f.x += dx / d * 3; f.y += dy / d * 3;
          break;
        }
      }
      if (f.st == DEAD || f.st == WAIT) continue;
      for (auto& s : mine)
        if (s.on && fabsf(s.x - f.x) < 11 && fabsf(s.y - f.y) < 9) { s.on = false; hitFoe(f); break; }
      if (f.st != DEAD && deadTimer == 0 && (f.st == DIVE || f.st == BACK) &&
          fabsf(f.x - px) < 15 && fabsf(f.y - playerY()) < 12) {
        f.st = DEAD; boom(f.x, f.y); killPlayer();
      }
    }

    // Sturzfluege starten, sobald alle eingeflogen sind
    if (inForm > 0 && deadTimer == 0 && --diveTimer <= 0) {
      diveTimer = max(18, 75 - level * 6) + irand(0, 30);
      int n = 1 + (level >= 2 && irand(0, 2) == 0);
      for (int k = 0; k < n; k++) {
        int pick = irand(0, nFoes - 1);
        for (int j = 0; j < nFoes; j++) {
          Foe& f = foes[(pick + j) % nFoes];
          if (f.st == FORM) { f.st = DIVE; f.ph = 0; f.shots = 0; break; }
        }
      }
    }

    for (auto& s : theirs) {
      if (!s.on) continue;
      s.x += s.vx; s.y += s.vy;
      if (s.y > SCR_H) s.on = false;
      else if (deadTimer == 0 && fabsf(s.x - px) < 9 && fabsf(s.y - playerY()) < 9) { s.on = false; killPlayer(); }
    }
    if (alive == 0) { waveWait = 50; score += 1000; }
  }

  void render() {
    for (auto& s : stars) cP((int)s.x, (int)s.y, s.v > 1.8f ? C_WHITE : s.v > 1.1f ? C_GRAY : C_DIM);
    bool fr = (tick / 10) % 2;
    for (auto& f : foes) {
      if (f.st == DEAD || f.st == WAIT) continue;
      const uint16_t* spr; uint16_t col;
      if (f.type == 0) { spr = fr ? CM1 : CM0; col = f.hp == 2 ? C_GREEN : C_PURPLE; }
      else if (f.type == 1) { spr = fr ? ST1 : ST0; col = C_RED; }
      else { spr = fr ? DR1 : DR0; col = C_BLUE; }
      cSpr(spr, 11, 8, (int)f.x - 11, (int)f.y - 8, col, 2);
      if (f.type == 2) cR((int)f.x - 1, (int)f.y - 4, 3, 3, C_YELLOW);
    }
    for (auto& s : mine) if (s.on) cR((int)s.x - 1, (int)s.y, 2, 7, C_YELLOW);
    for (auto& s : theirs) if (s.on) cR((int)s.x - 1, (int)s.y, 3, 5, C_PINK);
    for (auto& b : booms) {
      if (b.t <= 0) continue;
      int r = (12 - b.t) * 2;
      cCO((int)b.x, (int)b.y, r, b.t > 6 ? C_YELLOW : C_ORANGE);
      cCO((int)b.x, (int)b.y, r / 2, C_RED);
    }
    if (deadTimer == 0) {
      cSpr(SHIP, 11, 9, (int)px - 11, playerY() - 9, C_WHITE, 2);
      cR((int)px - 1, playerY() - 5, 3, 4, C_RED);
    }
    char lv[12];
    snprintf(lv, sizeof(lv), "STAGE %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives && i < 5; i++) cSpr(SHIP, 11, 9, 6 + i * 14, SCR_H - 10, C_CYAN, 1);
    if (waveWait > 0) cTxt("STAGE GESCHAFFT!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
    else if (tick < 60 && level == 0) cTxt("BEREIT MACHEN!", SCR_W / 2, SCR_H / 2, 2, C_CYAN);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
