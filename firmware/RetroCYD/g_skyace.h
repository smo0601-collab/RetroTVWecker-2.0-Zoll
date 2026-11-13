// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SKY ACE  -  Propellerflugzeug-Shooter ueber dem Ozean
// ============================================================================
//  Steuerkreuz/Stick: fliegen, A = feuern (halten = Dauerfeuer),
//  B = Looping (3x pro Leben, kurz unverwundbar).
//  Rote Staffel komplett abschiessen -> POW-Kraftpaket fuer staerkere Schuesse.
// ============================================================================
#pragma once

namespace SkyAce {
  const uint16_t PLANE[12] = {0x0080, 0x01C0, 0x01C0, 0x03E0, 0x7FFF, 0x7FFF, 0x3EBE, 0x0080, 0x01C0, 0x03E0, 0x07F0, 0x0080};
  const uint16_t FIGHTD[9] = {0x0020, 0x00F8, 0x0070, 0x0471, 0x07FF, 0x07FF, 0x0471, 0x0070, 0x0020};

  enum { FIGHTER, RED, BOMBER };
  struct Foe { float x, y, vx, vy, bx, by; uint8_t type, pat; int hp, t, shootT, idx; bool on; };
  struct Shot { float x, y, vx, vy; bool on; };
  struct Island { float x, y; int r; };
  struct Boom { float x, y; int t, size; };

  const int MAXF = 16, MAXS = 14, MAXE = 14;
  GArr<Foe, MAXF> foes;
  GArr<Shot, MAXS> shots;
  GArr<Shot, MAXE> ebul;
  Island isles[3];
  Boom booms[8];
  bool powOn; float powX, powY;

  float px, py;
  int lives, power, loops, loopT, invT, deadT, spawnT, bomberT, redLeft, redKilled, fireCd;
  uint32_t score;

  int lvl() { return min((int)(tick / 1800), 6); }

  void boom(float x, float y, int size) { for (auto& b : booms) if (b.t <= 0) { b = {x, y, 16, size}; return; } }

  Foe* newFoe() { for (auto& f : foes) if (!f.on) return &f; return nullptr; }

  void spawnWave() {
    int kind = irand(0, 9);
    if (kind < 3) {                       // Reihe von oben
      float x0 = frand(30, SCR_W - 30);
      for (int i = 0; i < 4; i++) {
        Foe* f = newFoe(); if (!f) return;
        *f = {x0, -15.0f - i * 26, 0, 2.2f + lvl() * 0.15f, x0, 0, FIGHTER, 0, 1, 0, irand(40, 120), i, true};
      }
    } else if (kind < 5) {                // Sturzflug in V-Form
      for (int i = 0; i < 5; i++) {
        Foe* f = newFoe(); if (!f) return;
        float x0 = SCR_W / 2 + (i - 2) * 24;
        *f = {x0, -15.0f - abs(i - 2) * 14, 0, 3.0f, x0, 0, FIGHTER, 1, 1, 0, irand(20, 60), i, true};
      }
    } else if (kind < 7) {                // von der Seite
      bool left = irand(0, 1);
      float y0 = frand(SCR_H * 0.12f, SCR_H * 0.35f);
      for (int i = 0; i < 4; i++) {
        Foe* f = newFoe(); if (!f) return;
        *f = {left ? -15.0f - i * 24 : SCR_W + 15.0f + i * 24, y0, left ? 2.4f : -2.4f, 0, 0, y0, FIGHTER, 2, 1, 0, irand(30, 90), i, true};
      }
    } else if (redLeft == 0 && !powOn) {  // rote Staffel
      redLeft = 5; redKilled = 0;
      float x0 = frand(SCR_W * 0.3f, SCR_W * 0.7f);
      for (int i = 0; i < 5; i++) {
        Foe* f = newFoe(); if (!f) { redLeft = 0; return; }
        *f = {x0, -15.0f - i * 20, 0, 1.7f, x0, 0, RED, 3, 1, 0, 999, i, true};
      }
    }
  }

  void start() {
    foes.alloc();
    shots.alloc();
    ebul.alloc();
    px = SCR_W / 2; py = SCR_H - 40;
    lives = 3; power = 1; loops = 3; loopT = 0; invT = 60; deadT = 0;
    spawnT = 40; bomberT = 900; redLeft = 0; redKilled = 0; fireCd = 0; score = 0; powOn = false;
    for (auto& f : foes) f.on = false;
    for (auto& s : shots) s.on = false;
    for (auto& s : ebul) s.on = false;
    for (auto& b : booms) b.t = 0;
    for (int i = 0; i < 3; i++) isles[i] = {frand(20, SCR_W - 20), (float)(i * SCR_H / 3), irand(18, 34)};
  }

  void fireAt(const Foe& f, float speed, float spread) {
    for (auto& s : ebul) {
      if (s.on) continue;
      float a = atan2f(py - f.y, px - f.x) + spread;
      s = {f.x, f.y + 6, cosf(a) * speed, sinf(a) * speed, true};
      return;
    }
  }

  void addShot(float x, float vx) {
    for (auto& s : shots) if (!s.on) { s = {x, py - 14, vx, -8, true}; return; }
  }

  void killPlayer() {
    if (invT > 0 || loopT > 0 || deadT > 0) return;
    boom(px, py, 2);
    lives--; deadT = 60; power = max(1, power - 1);
    Snd::big(Snd::BOOM);
    rumble(500, 0xC0);
  }

  void hitFoe(Foe& f, int dmg) {
    f.hp -= dmg;
    if (f.hp > 0) { Snd::fx(Snd::TICK); rumble(20, 0x30); return; }
    f.on = false;
    boom(f.x, f.y, f.type == BOMBER ? 3 : 1);
    if (f.type == BOMBER) { score += 1500; Snd::fx(Snd::CRASH); rumble(300, 0xA0); }
    else if (f.type == RED) {
      score += 100; redKilled++; redLeft = max(0, redLeft - 1);
      if (redKilled == 5) { powOn = true; powX = f.x; powY = f.y; score += 500; }
    } else score += 50;
    Snd::fx(Snd::THUD);
    rumble(40, 0x50);
  }

  void update() {
    for (auto& i : isles) { i.y += 1.0f; if (i.y - i.r > SCR_H) { i.y = -i.r - irand(0, 80); i.x = frand(20, SCR_W - 20); i.r = irand(18, 34); } }
    for (auto& b : booms) if (b.t > 0) b.t--;

    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        px = SCR_W / 2; py = SCR_H - 40; invT = 90; loops = 3;
      }
    } else {
      float sp = 3.0f;
      float mx = 0, my = 0;
      if (cur.left) mx = -1;
      if (cur.right) mx = 1;
      if (cur.up) my = -1;
      if (cur.down) my = 1;
      if (abs(cur.ax) > 120 && mx == 0) mx = constrain(cur.ax / 400.0f, -1.0f, 1.0f);
      if (abs(cur.ay) > 120 && my == 0) my = constrain(cur.ay / 400.0f, -1.0f, 1.0f);
      px = constrain(px + mx * sp, 14.0f, (float)SCR_W - 14);
      py = constrain(py + my * sp, 40.0f, (float)SCR_H - 16);
      if (invT > 0) invT--;
      if (loopT > 0) loopT--;
      if (E.b && loops > 0 && loopT == 0) { loopT = 45; loops--; Snd::fx(Snd::LOOP); rumble(100, 0x50); }
      if (fireCd > 0) fireCd--;
      if ((E.a || cur.a) && fireCd == 0 && loopT == 0) {
        fireCd = 5;
        Snd::fx(Snd::SHOOT, Snd::P_AUTO);
        addShot(px - 5, 0); addShot(px + 5, 0);
        if (power >= 2) { addShot(px - 8, -1.2f); addShot(px + 8, 1.2f); }
        if (power >= 3) { addShot(px - 10, -2.4f); addShot(px + 10, 2.4f); }
      }
    }
    for (auto& s : shots) if (s.on) { s.x += s.vx; s.y += s.vy; if (s.y < -10 || s.x < -10 || s.x > SCR_W + 10) s.on = false; }

    // Gegnerwellen
    if (--spawnT <= 0) { spawnWave(); spawnT = max(45, 110 - lvl() * 10) + irand(0, 40); }
    if (--bomberT <= 0) {
      bomberT = max(500, 1000 - lvl() * 60);
      Foe* f = newFoe();
      if (f) *f = {(float)SCR_W / 2, -30, 0, 0.7f, (float)SCR_W / 2, 0, BOMBER, 4, 22 + lvl() * 4, 0, 60, 0, true};
    }

    for (auto& f : foes) {
      if (!f.on) continue;
      f.t++;
      switch (f.pat) {
        case 0: f.y += f.vy; f.x = f.bx + sinf(f.t * 0.06f + f.idx) * 14; break;
        case 1:
          if (f.vy > 0 && f.y < SCR_H * 0.55f) { f.y += f.vy; f.x += constrain(px - f.x, -1.6f, 1.6f); }
          else { f.vy = -2.6f; f.y += f.vy; f.x += (f.x < SCR_W / 2 ? -1 : 1) * 1.5f; }
          break;
        case 2: f.x += f.vx; f.y = f.by + sinf(f.t * 0.07f) * 22; break;
        case 3: f.y += f.vy; f.x = f.bx + sinf(f.t * 0.045f + f.idx * 0.5f) * SCR_W * 0.3f; break;
        case 4:   // Bomber
          if (f.y < SCR_H * 0.18f) f.y += f.vy;
          else f.x = f.bx + sinf(f.t * 0.02f) * SCR_W * 0.3f;
          if (f.t > 700) f.y -= 1.5f;
          break;
      }
      if (f.y > SCR_H + 30 || f.y < -60 || f.x < -60 || f.x > SCR_W + 60) {
        if (f.type == RED) redLeft = max(0, redLeft - 1);
        f.on = false; continue;
      }
      if (--f.shootT <= 0 && deadT == 0 && f.y > 0 && f.y < SCR_H * 0.6f) {
        if (f.type == BOMBER) { fireAt(f, 2.6f, -0.3f); fireAt(f, 2.6f, 0); fireAt(f, 2.6f, 0.3f); f.shootT = max(35, 70 - lvl() * 5); }
        else { fireAt(f, 3.0f + lvl() * 0.2f, frand(-0.1f, 0.1f)); f.shootT = irand(80, 160) - lvl() * 8; }
      }
      float hw = f.type == BOMBER ? 30 : 11, hh = f.type == BOMBER ? 14 : 9;
      for (auto& s : shots)
        if (s.on && fabsf(s.x - f.x) < hw && fabsf(s.y - f.y) < hh) { s.on = false; hitFoe(f, 1); if (!f.on) break; }
      if (f.on && deadT == 0 && fabsf(f.x - px) < hw + 8 && fabsf(f.y - py) < hh + 6) {
        if (loopT == 0 && invT == 0) { if (f.type != BOMBER) { f.on = false; boom(f.x, f.y, 1); if (f.type == RED) redLeft = max(0, redLeft - 1); } killPlayer(); }
      }
    }
    for (auto& s : ebul) {
      if (!s.on) continue;
      s.x += s.vx; s.y += s.vy;
      if (s.y > SCR_H || s.y < -10 || s.x < -10 || s.x > SCR_W + 10) s.on = false;
      else if (deadT == 0 && fabsf(s.x - px) < 7 && fabsf(s.y - py) < 7) { s.on = false; killPlayer(); }
    }
    if (powOn) {
      powY += 0.8f;
      if (powY > SCR_H + 10) powOn = false;
      else if (deadT == 0 && fabsf(powX - px) < 14 && fabsf(powY - py) < 14) {
        powOn = false; power = min(3, power + 1); score += 300; Snd::fx(Snd::POWER); rumble(200, 0x70);
      }
    }
  }

  void render() {
    canvas.fillScreen(RGB(20, 60, 150));
    for (int i = 0; i < 24; i++) {     // Wellen
      int x = (i * 53) % SCR_W, y = (int)((i * 41 + tick * 1) % (SCR_H + 20)) - 10;
      cH(x, y, 6, RGB(60, 110, 200));
    }
    for (auto& i : isles) {
      cC((int)i.x, (int)i.y, i.r + 4, RGB(220, 200, 130));
      cC((int)i.x, (int)i.y, i.r, RGB(40, 140, 60));
      cC((int)i.x - i.r / 3, (int)i.y - i.r / 3, i.r / 3, RGB(60, 170, 80));
    }
    for (auto& f : foes) {
      if (!f.on) continue;
      int x = (int)f.x, y = (int)f.y;
      if (f.type == BOMBER) {
        cR(x - 34, y - 3, 68, 9, RGB(70, 90, 70));
        cR(x - 7, y - 16, 14, 34, RGB(90, 110, 90));
        cR(x - 16, y + 12, 32, 5, RGB(70, 90, 70));
        for (int k = -1; k <= 1; k += 2) { cC(x + k * 20, y + 2, 5, RGB(50, 60, 50)); cH(x + k * 20 - 7, y + 8, 14, C_GRAY); }
        cR(x - 30, y - 22, max(0, min(60, (60 * f.hp) / (22 + lvl() * 4))), 2, C_RED);
      } else {
        cSpr(FIGHTD, 11, 9, x - 11 + 6, y - 9 + 6, RGB(10, 40, 100), 2);   // Schatten
        cSpr(FIGHTD, 11, 9, x - 11, y - 9, f.type == RED ? C_RED : RGB(90, 160, 90), 2);
        if ((tick / 2) % 2) cH(x - 5, y + 9, 10, C_GRAY);   // Propeller
      }
    }
    if (powOn) {
      cRR((int)powX - 10, (int)powY - 7, 20, 14, 3, (tick / 4) % 2 ? C_RED : C_ORANGE);
      cTxt("POW", (int)powX, (int)powY + 1, 1, C_WHITE);
    }
    for (auto& s : shots) if (s.on) cR((int)s.x - 1, (int)s.y - 3, 2, 6, C_YELLOW);
    for (auto& s : ebul) if (s.on) { cC((int)s.x, (int)s.y, 2, C_PINK); }
    for (auto& b : booms) {
      if (b.t <= 0) continue;
      int r = (16 - b.t) * (b.size + 1);
      cC((int)b.x, (int)b.y, r / 2 + 1, b.t > 8 ? C_YELLOW : C_ORANGE);
      cCO((int)b.x, (int)b.y, r, C_RED);
    }
    if (deadT == 0 && (invT == 0 || (tick / 3) % 2)) {
      int s = 2;
      if (loopT > 0) s = (loopT > 10 && loopT < 35) ? 1 : 2;
      int w = 15 * s, h = 12 * s;
      cSpr(PLANE, 15, 12, (int)px - w / 2 + 8, (int)py - h / 2 + 8, RGB(10, 40, 100), s);
      cSpr(PLANE, 15, 12, (int)px - w / 2, (int)py - h / 2, loopT > 0 ? C_WHITE : RGB(230, 230, 240), s);
      if ((tick / 2) % 2) cH((int)px - 6, (int)py - h / 2 - 1, 12, C_GRAY);
    }
    cHud(score);
    for (int i = 0; i < lives - 1 && i < 4; i++) cSpr(PLANE, 15, 12, 6 + i * 18, SCR_H - 14, C_WHITE, 1);
    for (int i = 0; i < loops; i++) cTxt("L", SCR_W - 10 - i * 10, SCR_H - 12, 1, C_CYAN);
    if (tick < 60) cTxt("STARTKLAR!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
