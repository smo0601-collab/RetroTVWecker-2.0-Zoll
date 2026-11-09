// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  PUFF JUMP  -  Blip, ein kleiner Wolken-Kobold, kann schweben, Gegner
//                einsaugen und als Stern wieder ausspucken
// ============================================================================
//  links/rechts laufen, A = springen (in der Luft: flattern/schweben),
//  B halten = einsaugen, B mit vollem Mund = Stern spucken
// ============================================================================
#pragma once

namespace Puff {
  struct Plat { float fx, fy, fw; };
  const Plat STAGES[3][6] = {
    {{0.08f, 0.72f, 0.30f}, {0.62f, 0.72f, 0.30f}, {0.30f, 0.50f, 0.40f}, {0.04f, 0.28f, 0.28f}, {0.68f, 0.28f, 0.28f}, {0, 0, 0}},
    {{0.00f, 0.70f, 0.36f}, {0.64f, 0.70f, 0.36f}, {0.34f, 0.54f, 0.32f}, {0.08f, 0.36f, 0.30f}, {0.62f, 0.36f, 0.30f}, {0.34f, 0.18f, 0.32f}},
    {{0.05f, 0.76f, 0.25f}, {0.30f, 0.60f, 0.25f}, {0.55f, 0.44f, 0.25f}, {0.75f, 0.28f, 0.22f}, {0.10f, 0.36f, 0.30f}, {0.40f, 0.20f, 0.20f}},
  };
  const int NPLAT = 6;

  enum { WALKER, HOPPER, FLYER };
  struct Foe { float x, y, vx, vy, baseY, ph; uint8_t type; bool on, pulled; int stunT; };
  struct Star { float x, y, vx; bool on; };
  const int MAXF = 8;
  Foe foes[MAXF];
  Star star;
  struct Spark { float x, y, vx, vy; int t; };
  Spark sparks[16];

  float px, py, vx, vy;
  bool ground, inhaling, full, floating;
  int face, flaps, hp, lives, stage, invT, deadT, winT, spawnLeft;
  uint32_t score;
  int top, bottom;

  int platX(int i) { return (int)(STAGES[stage % 3][i].fx * SCR_W); }
  int platW(int i) { return (int)(STAGES[stage % 3][i].fw * SCR_W); }
  int platY(int i) { return top + (int)(STAGES[stage % 3][i].fy * (bottom - top)); }

  void spark(float x, float y, int n, int life) {
    for (int k = 0; k < n; k++)
      for (auto& s : sparks)
        if (s.t <= 0) { float a = frand(0, 6.283f), v = frand(1, 3); s = {x, y, cosf(a) * v, sinf(a) * v, life}; break; }
  }

  void spawnFoe() {
    for (auto& f : foes) {
      if (f.on) continue;
      f.type = irand(0, 2);
      if (stage == 0 && f.type == FLYER && irand(0, 1)) f.type = WALKER;
      f.on = true; f.pulled = false; f.stunT = 0;
      f.vx = (irand(0, 1) ? 1 : -1) * (0.7f + stage * 0.12f);
      f.vy = 0; f.ph = frand(0, 6);
      if (f.type == FLYER) {
        f.x = f.vx > 0 ? -10 : SCR_W + 10;
        f.baseY = frand(top + 30, bottom - 60); f.y = f.baseY;
      } else {
        int p = irand(0, NPLAT - 1);
        if (STAGES[stage % 3][p].fw == 0) p = 0;
        f.x = platX(p) + platW(p) / 2; f.y = platY(p);
      }
      return;
    }
  }

  void startStage() {
    for (auto& f : foes) f.on = false;
    spawnLeft = 6 + stage * 2;
    for (int i = 0; i < 3; i++) { spawnFoe(); spawnLeft--; }
    star.on = false;
    px = SCR_W / 2; py = bottom; vx = vy = 0; ground = true; full = false; inhaling = false;
  }

  void start() {
    top = 20; bottom = SCR_H - 12;
    hp = 3; lives = 3; stage = 0; score = 0; invT = 0; deadT = 0; winT = 0; face = 1;
    for (auto& s : sparks) s.t = 0;
    startStage();
  }

  // Landen auf Plattformen (von oben) oder auf dem Boden
  bool landOn(float x, float& y, float& v, float prevY) {
    if (y >= bottom) { y = bottom; v = 0; return true; }
    if (v < 0) return false;
    for (int i = 0; i < NPLAT; i++) {
      if (STAGES[stage % 3][i].fw == 0) continue;
      int pyy = platY(i);
      if (prevY <= pyy && y >= pyy && x > platX(i) - 2 && x < platX(i) + platW(i) + 2) {
        y = pyy; v = 0; return true;
      }
    }
    return false;
  }

  void hurt() {
    if (invT > 0 || deadT > 0) return;
    hp--;
    invT = 60;
    vy = -3; vx = -face * 3;
    rumble(250, 0xA0);
    if (hp <= 0) { lives--; deadT = 60; spark(px, py - 8, 12, 25); }
  }

  void defeat(Foe& f, uint32_t pts) {
    f.on = false;
    score += pts;
    spark(f.x, f.y - 6, 8, 18);
    rumble(60, 0x60);
    if (spawnLeft > 0) { spawnFoe(); spawnLeft--; }
  }

  void update() {
    for (auto& s : sparks) if (s.t > 0) { s.x += s.vx; s.y += s.vy; s.t--; }
    if (winT > 0) { if (--winT == 0) { stage++; startStage(); } return; }
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        hp = 3; invT = 90; px = SCR_W / 2; py = bottom; vx = vy = 0; full = false;
      }
      return;
    }
    if (invT > 0) invT--;

    // Bewegung
    float acc = ground ? 0.6f : 0.35f;
    if (cur.left)  { vx -= acc; face = -1; }
    if (cur.right) { vx += acc; face = 1; }
    float maxv = full ? 1.6f : 2.4f;
    vx = constrain(vx, -maxv, maxv);
    if (!cur.left && !cur.right) vx *= ground ? 0.6f : 0.92f;
    inhaling = cur.b && !full;

    if (E.a) {
      if (ground) { vy = -6.2f; ground = false; flaps = 0; }
      else if (!full && flaps < 6) { vy = -3.2f; flaps++; floating = true; rumble(15, 0x20); }
    }
    if (full && E.b) {     // Stern spucken
      full = false;
      star = {px + face * 10, py - 8, face * 6.5f, true};
      rumble(80, 0x60);
    }
    float g = 0.38f;
    vy += g;
    float maxFall = (floating && cur.a && !full) ? 1.0f : 5.0f;
    if (vy > maxFall) vy = maxFall;
    float prevY = py;
    px = constrain(px + vx, 8.0f, (float)SCR_W - 8);
    py += vy;
    bool was = ground;
    ground = landOn(px, py, vy, prevY);
    if (ground) { floating = false; flaps = 0; if (!was) spark(px, py, 2, 6); }
    if (py < top + 10) { py = top + 10; vy = 0; }

    // Stern
    if (star.on) {
      star.x += star.vx;
      if (star.x < -10 || star.x > SCR_W + 10) star.on = false;
    }

    // Gegner
    int active = 0;
    for (auto& f : foes) {
      if (!f.on) continue;
      active++;
      f.ph += 0.08f;
      float dx = px - f.x, dy = (py - 8) - (f.y - 7);
      bool inFront = dx * face < 0 && fabsf(dx) < 70 && fabsf(dy) < 22;
      if (inhaling && inFront) {
        f.pulled = true;
        f.x += (dx > 0 ? 1 : -1) * 3.2f;
        f.y += dy * 0.12f;
        if (fabsf(px - f.x) < 10 && fabsf(dy) < 14) { f.on = false; full = true; score += 50; rumble(60, 0x50);
          if (spawnLeft > 0) { spawnFoe(); spawnLeft--; } continue; }
      } else {
        f.pulled = false;
        if (f.type == FLYER) {
          f.x += f.vx;
          f.y = f.baseY + sinf(f.ph) * 18;
          if (f.x < -20 || f.x > SCR_W + 20) { f.vx = -f.vx; }
        } else {
          float prev = f.y;
          f.vy += 0.38f;
          if (f.type == HOPPER && f.vy == 0.38f && irand(0, 40) == 0) f.vy = -5;
          f.x += f.vx;
          f.y += f.vy;
          if (f.x < 8 || f.x > SCR_W - 8) f.vx = -f.vx;
          bool onG = landOn(f.x, f.y, f.vy, prev);
          // an Plattformkanten umdrehen
          if (onG && f.type == WALKER && f.y < bottom) {
            bool edge = true;
            for (int i = 0; i < NPLAT; i++) {
              if (STAGES[stage % 3][i].fw == 0 || platY(i) != (int)f.y) continue;
              float nx = f.x + f.vx * 6;
              if (nx > platX(i) && nx < platX(i) + platW(i)) edge = false;
            }
            if (edge) f.vx = -f.vx;
          }
        }
      }
      if (star.on && fabsf(star.x - f.x) < 12 && fabsf(star.y - (f.y - 7)) < 14) { defeat(f, 200); continue; }
      if (!inhaling && fabsf(px - f.x) < 12 && fabsf(dy) < 14) hurt();
    }
    if (active == 0 && spawnLeft <= 0 && winT == 0) { winT = 60; score += 1000; }
    else if (active < 2 + stage / 2 && spawnLeft > 0 && tick % 45 == 0) { spawnFoe(); spawnLeft--; }
  }

  void drawStar(int x, int y, int r, uint16_t c) {
    for (int k = 0; k < 5; k++) {
      float a = tick * 0.2f + k * 1.2566f;
      cL(x, y, x + (int)(cosf(a) * r), y + (int)(sinf(a) * r), c);
    }
    cC(x, y, r / 3, c);
  }

  void render() {
    // Himmel mit Wolken
    for (int i = 0; i < 4; i++) {
      int cx = (int)((i * 97 + tick / 4) % (SCR_W + 60)) - 30, cy = top + 20 + i * 37 % (bottom - top - 40);
      cC(cx, cy, 10, C_PANEL); cC(cx + 12, cy + 2, 8, C_PANEL); cC(cx - 10, cy + 3, 7, C_PANEL);
    }
    for (int i = 0; i < NPLAT; i++) {
      if (STAGES[stage % 3][i].fw == 0) continue;
      cRR(platX(i), platY(i), platW(i), 7, 3, RGB(90, 170, 70));
      cH(platX(i) + 2, platY(i) + 6, platW(i) - 4, RGB(60, 110, 40));
    }
    cR(0, bottom, SCR_W, SCR_H - bottom, RGB(90, 170, 70));

    for (auto& f : foes) {
      if (!f.on) continue;
      int x = (int)f.x, y = (int)f.y;
      bool fl = f.vx < 0;
      if (f.type == WALKER) {        // Raupe
        for (int k = 2; k >= 0; k--) cC(x + (fl ? k : -k) * 5, y - 5 - (k == 0 ? 2 : 0) + (int)(sinf(f.ph * 3 + k) * 1.5f), 5, k ? C_DGREEN : C_GREEN);
        cP(x + (fl ? -2 : 2), y - 9, C_BG);
      } else if (f.type == HOPPER) { // huepfender Wuerfel
        cRR(x - 7, y - 14, 14, 14, 3, C_PURPLE);
        cR(x - 4, y - 10, 3, 3, C_WHITE); cR(x + 1, y - 10, 3, 3, C_WHITE);
      } else {                       // Flatterling
        int w = (tick / 4) % 2 ? 9 : 5;
        cTri(x, y - 7, x - 12, y - 7 - w, x - 4, y - 4, C_RED);
        cTri(x, y - 7, x + 12, y - 7 - w, x + 4, y - 4, C_RED);
        cC(x, y - 7, 5, C_ORANGE);
        cP(x - 2, y - 8, C_BG); cP(x + 2, y - 8, C_BG);
      }
    }
    if (star.on) drawStar((int)star.x, (int)star.y, 8, C_YELLOW);
    for (auto& s : sparks) if (s.t > 0) cP((int)s.x, (int)s.y, s.t > 6 ? C_YELLOW : C_ORANGE);

    // Blip
    if (deadT == 0 && (invT == 0 || (tick / 3) % 2)) {
      int x = (int)px, y = (int)py;
      int r = full ? 10 : (floating && !ground) ? 9 : 8;
      uint16_t body = RGB(80, 210, 200);
      cC(x, y - r, r, body);
      cC(x - face * 3, y - r - 3, 2, C_WHITE);       // Glanz
      cR(x + face * 2 - 1, y - r - 4, 2, 4, C_BG);   // Auge
      cR(x + face * 6 - 1, y - r - 4, 2, 4, C_BG);
      cL(x, y - 2 * r, x - face * 3, y - 2 * r - 5, body);   // Antenne
      cC(x - face * 3, y - 2 * r - 6, 2, C_YELLOW);
      cE(x - 5, y - 1, 4, 2, RGB(40, 120, 200));     // Fuesse
      cE(x + 5, y - 1, 4, 2, RGB(40, 120, 200));
      if (inhaling) {
        cC(x + face * (r - 1), y - r + 2, 4, C_BG);
        for (int k = 0; k < 3; k++) {
          int lx = x + face * (r + 10 + ((tick * 3 + k * 12) % 40));
          cH(face > 0 ? lx : lx - 8, y - r - 4 + k * 4, 8, C_GRAY);
        }
      }
    }
    char lv[12];
    snprintf(lv, sizeof(lv), "STAGE %d", stage + 1);
    cHud(score, lv);
    for (int i = 0; i < 3; i++) cC(10 + i * 12, 22, 4, i < hp ? C_RED : C_DIM);
    for (int i = 0; i < lives - 1 && i < 4; i++) cC(SCR_W - 70 - i * 12, 8, 4, RGB(80, 210, 200));
    if (winT > 0) cTxt("STAGE GESCHAFFT!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
