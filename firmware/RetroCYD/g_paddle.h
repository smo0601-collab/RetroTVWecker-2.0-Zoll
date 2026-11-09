// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  PADDLE  -  Tischtennis fuer 1 Spieler (gegen den Computer) oder 2 Spieler
// ============================================================================
//  Quer: Schlaeger links/rechts, Steuerung hoch/runter
//  Hochkant: Schlaeger unten/oben, Steuerung links/rechts
//  Sind 2 Controller verbunden, spielt der zweite den oberen/rechten Schlaeger.
// ============================================================================
#pragma once

namespace Paddle {
  const float PL = 42, PT = 6, BS = 6, WIN = 7;

  // Logische Koordinaten: lx = Achse zwischen den Spielern, ly = Bewegungsachse
  float LXM, LYmin, LYmax;
  float bx, by, vx, vy;
  float p1, p2, cpuErr;
  int s1, s2, level, serveWait, msgTimer;
  uint32_t total;
  bool twoP;
  char msg[24];

  void rectL(float lx, float ly, float w, float h, uint16_t c) {
    if (landscape()) cR((int)lx, (int)ly, (int)w, (int)h, c);
    else cR((int)ly, (int)(SCR_H - lx - w), (int)h, (int)w, c);
  }

  void serve(int dir) {
    bx = LXM / 2 - BS / 2;
    by = (LYmin + LYmax) / 2 - BS / 2;
    float sp = 3.6f + level * 0.35f;
    float ang = frand(-0.45f, 0.45f);
    vx = dir * sp * cosf(ang);
    vy = sp * sinf(ang);
    serveWait = 30;
  }

  void start() {
    twoP = numPlayers >= 2;
    LXM   = landscape() ? SCR_W : SCR_H - 16;
    LYmin = landscape() ? 16 : 0;
    LYmax = landscape() ? SCR_H : SCR_W;
    p1 = p2 = (LYmin + LYmax) / 2;
    s1 = s2 = 0;
    level = 0;
    total = 0;
    cpuErr = 0;
    msgTimer = 45;
    snprintf(msg, sizeof(msg), twoP ? "2 SPIELER" : "BIS %d PUNKTE", (int)WIN);
    serve(irand(0, 1) ? 1 : -1);
  }

  float axisInput(const Keys& k) {
    bool minus = landscape() ? k.up : k.left;
    bool plus  = landscape() ? k.down : k.right;
    float v = 0;
    if (minus) v = -1;
    if (plus) v = 1;
    int an = landscape() ? k.ay : k.ax;
    if (abs(an) > 80) v = constrain(an / 380.0f, -1.0f, 1.0f);
    return v;
  }

  void bounce(float pc, int dir) {
    float off = constrain((by + BS / 2 - pc) / (PL / 2), -1.0f, 1.0f);
    float sp = sqrtf(vx * vx + vy * vy) * 1.06f;
    if (sp > 11) sp = 11;
    float ang = off * 1.0f;
    vx = dir * sp * cosf(ang);
    vy = sp * sinf(ang);
    float skill = 1.0f - min(level, 8) / 10.0f;
    cpuErr = frand(-14, 14) * skill;
  }

  void pointFor(int who) {
    if (who == 1) {
      s1++;
      if (!twoP) total += 100;
      rumbleFor(1, 200, 0x80);
    } else {
      s2++;
      rumbleFor(0, 200, 0x80);
    }
    if (s1 >= WIN) {
      if (twoP) { gameOverText("SPIELER 1 SIEGT", 0); return; }
      total += 1000;
      level++;
      s1 = s2 = 0;
      snprintf(msg, sizeof(msg), "LEVEL %d", level + 1);
      msgTimer = 60;
    } else if (s2 >= WIN) {
      if (twoP) gameOverText("SPIELER 2 SIEGT", 0);
      else gameOver(total);
      return;
    }
    serve(who == 1 ? 1 : -1);
  }

  void update() {
    if (msgTimer > 0) msgTimer--;
    const Keys& k1 = twoP ? pk[0] : cur;
    p1 += axisInput(k1) * 7.5f;
    if (twoP) {
      p2 += axisInput(pk[1]) * 7.5f;
    } else {
      float target = (vx > 0) ? by + BS / 2 + cpuErr : (LYmin + LYmax) / 2;
      float sp = min(7.5f, 2.8f + level * 0.55f);
      p2 += constrain(target - p2, -sp, sp);
    }
    p1 = constrain(p1, LYmin + PL / 2, LYmax - PL / 2);
    p2 = constrain(p2, LYmin + PL / 2, LYmax - PL / 2);

    if (serveWait > 0) { serveWait--; return; }

    bx += vx;
    by += vy;
    if (by < LYmin)       { by = LYmin;      vy = fabsf(vy); }
    if (by + BS > LYmax)  { by = LYmax - BS; vy = -fabsf(vy); }

    float x1 = 8, x2 = LXM - 8 - PT;
    if (vx < 0 && bx <= x1 + PT && bx + BS >= x1 && by + BS >= p1 - PL / 2 && by <= p1 + PL / 2) {
      bx = x1 + PT;
      bounce(p1, 1);
      rumbleFor(0, 40, 0x50);
    }
    if (vx > 0 && bx + BS >= x2 && bx <= x2 + PT && by + BS >= p2 - PL / 2 && by <= p2 + PL / 2) {
      bx = x2 - BS;
      bounce(p2, -1);
      if (twoP) rumbleFor(1, 40, 0x50);
    }
    if (bx < -12) pointFor(2);
    else if (bx > LXM + 12) pointFor(1);
  }

  void render() {
    for (float ly = LYmin + 3; ly < LYmax; ly += 14) rectL(LXM / 2 - 1, ly, 2, 7, C_DIM);
    // Spielstand gross in den Spielhaelften
    char b1[4], b2[4];
    snprintf(b1, sizeof(b1), "%d", s1);
    snprintf(b2, sizeof(b2), "%d", s2);
    if (landscape()) {
      cTxt(b1, SCR_W / 4, 40, 4, C_GRID);
      cTxt(b2, SCR_W * 3 / 4, 40, 4, C_GRID);
    } else {
      cTxt(b1, SCR_W / 2, SCR_H * 3 / 4, 4, C_GRID);
      cTxt(b2, SCR_W / 2, SCR_H / 4 + 8, 4, C_GRID);
    }
    rectL(8, p1 - PL / 2, PT, PL, C_CYAN);
    rectL(LXM - 8 - PT, p2 - PL / 2, PT, PL, C_PINK);
    if (serveWait == 0 || (tick / 4) % 2) rectL(bx, by, BS, BS, C_YELLOW);

    if (twoP) cTxt("1  vs  2", SCR_W / 2, 4, 1, C_CYAN, textdatum_t::top_center);
    else {
      char lv[12];
      snprintf(lv, sizeof(lv), "LEVEL %d", level + 1);
      cHud(total, lv);
    }
    if (msgTimer > 0) cTxt(msg, SCR_W / 2, SCR_H / 2 - 30, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
