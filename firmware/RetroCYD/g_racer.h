// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SUNSET RACE  -  Pseudo-3D-Rennen in den Sonnenuntergang
// ============================================================================
//  links/rechts lenken, A (oder hoch) = Gas, B (oder runter) = Bremse
//  Vor Ablauf der Zeit den naechsten Checkpoint erreichen (+25 Sekunden).
//  Neben der Strasse wird man langsam, Autos und Palmen bremsen hart.
// ============================================================================
#pragma once

namespace Racer {
  const float SEGLEN = 200, ROADW = 2000, CAMH = 1000, DEPTH = 0.84f, PLAYERZ = 520;
  const float MAXSPD = 600;          // Welt-Einheiten pro Spielschritt
  const int DRAW = 48;
  const int CHECKPOINT = 1500;       // Segmente zwischen zwei Checkpoints

  struct Section { int len; int8_t curve; };
  const Section TRACK[] = {
    {60, 0}, {40, 2}, {30, 4}, {40, 0}, {50, -3}, {25, -5}, {40, 0}, {60, 3},
    {30, 0}, {35, -6}, {45, -2}, {50, 0}, {30, 5}, {30, 2}, {40, -4}, {40, 0},
  };
  const int NSEC = sizeof(TRACK) / sizeof(TRACK[0]);
  int trackLen;

  struct Car { float z, x, spd; uint16_t col; };
  const int NCARS = 7;
  Car cars[NCARS];

  struct Proj { int x, y, w; };

  float z, pX, spd, bgX, steerVis;
  int timeLeft, nextCheck, crashT, msgT, horizon;
  uint32_t score, meters, bonus;
  char msg[20];

  float curveAt(int seg) {
    int s = seg % trackLen;
    for (int i = 0; i < NSEC; i++) {
      if (s < TRACK[i].len) return TRACK[i].curve;
      s -= TRACK[i].len;
    }
    return 0;
  }

  void placeCar(Car& c, float ahead) {
    const uint16_t cols[5] = {C_BLUE, C_YELLOW, C_WHITE, C_GREEN, C_PURPLE};
    c.z = z + ahead;
    c.x = frand(-0.7f, 0.7f);
    c.spd = frand(0.25f, 0.55f) * MAXSPD;
    c.col = cols[irand(0, 4)];
  }

  void start() {
    trackLen = 0;
    for (auto& s : TRACK) trackLen += s.len;
    z = 0; pX = 0; spd = 0; bgX = 0; steerVis = 0;
    timeLeft = 50 * 30; nextCheck = CHECKPOINT; crashT = 0; score = 0; meters = 0; bonus = 0;
    horizon = (int)(SCR_H * 0.42f);
    for (int i = 0; i < NCARS; i++) placeCar(cars[i], 3000 + i * 1600);
    snprintf(msg, sizeof(msg), "LOS GEHT'S!");
    msgT = 45;
  }

  void update() {
    if (msgT > 0) msgT--;
    if (crashT > 0) crashT--;
    float sp = spd / MAXSPD;
    bool gas = cur.a || cur.up, brake = cur.b || cur.down;
    if (crashT == 0) {
      if (gas) spd += MAXSPD / 110;
      else if (brake) spd -= MAXSPD / 35;
      else spd -= MAXSPD / 300;
    }
    float steer = 0;
    if (cur.left) steer = -1;
    if (cur.right) steer = 1;
    if (abs(cur.ax) > 120 && !cur.left && !cur.right) steer = constrain(cur.ax / 400.0f, -1.0f, 1.0f);
    steerVis += (steer - steerVis) * 0.3f;
    int seg = (int)((z + PLAYERZ) / SEGLEN);
    float curve = curveAt(seg);
    pX += steer * (0.02f + 0.06f * sp) * (sp > 0.02f ? 1 : 0);
    pX -= curve * sp * sp * 0.008f;
    bgX += curve * sp * 0.6f;

    if (fabsf(pX) > 1.0f) {                  // neben der Strasse
      if (spd > MAXSPD / 3) spd -= MAXSPD / 40;
      if (tick % 8 == 0 && sp > 0.1f) rumble(60, 0x40);
      // Palmen stehen bei |x| ~ 1.6
      if (fabsf(pX) > 1.35f && seg % 7 == 0 && sp > 0.15f) {
        spd *= 0.1f; crashT = 30; rumble(400, 0xD0); pX *= 0.6f;
      }
    }
    pX = constrain(pX, -2.2f, 2.2f);
    spd = constrain(spd, 0.0f, MAXSPD);
    z += spd;

    for (auto& c : cars) {
      c.z += c.spd;
      float rel = c.z - (z + PLAYERZ);
      if (rel < -2000) placeCar(c, DRAW * SEGLEN * frand(0.7f, 1.0f));
      else if (rel > DRAW * SEGLEN * 1.5f) placeCar(c, DRAW * SEGLEN * frand(0.7f, 1.0f));
      if (crashT == 0 && rel > -60 && rel < 140 && fabsf(c.x - pX) < 0.42f) {
        spd = min(spd, c.spd * 0.5f);
        crashT = 25;
        c.z = z + PLAYERZ + 260;
        rumble(350, 0xC0);
      }
    }

    // Zeit und Checkpoints
    if (--timeLeft <= 0) { gameOver(score); return; }
    int dist = (int)(z / SEGLEN);
    meters = dist * 5;                     // ca. 5 m pro Segment
    if (dist >= nextCheck) {
      nextCheck += CHECKPOINT;
      timeLeft += 25 * 30;
      bonus += 1000;
      snprintf(msg, sizeof(msg), "CHECKPOINT!");
      msgT = 60;
      rumble(200, 0x70);
    }
    score = meters + bonus;
  }

  void quad(int x1, int y1, int w1, int x2, int y2, int w2, uint16_t c) {
    if (y1 == y2) return;
    cTri(x1 - w1, y1, x1 + w1, y1, x2 + w2, y2, c);
    cTri(x1 - w1, y1, x2 + w2, y2, x2 - w2, y2, c);
  }

  void drawPalm(int x, int y, float s) {
    int h = (int)(s * 90), w = max(1, (int)(s * 8));
    if (h < 3) return;
    cR(x - w / 2, y - h, w, h, RGB(130, 80, 40));
    int r = max(2, (int)(s * 22));
    cE(x - r / 2, y - h, r, r / 3 + 1, C_DGREEN);
    cE(x + r / 2, y - h, r, r / 3 + 1, C_DGREEN);
    cE(x, y - h - r / 3, r * 2 / 3, r / 2 + 1, C_GREEN);
  }

  void drawCarSprite(int x, int y, int w, uint16_t col) {
    int h = w * 55 / 100;
    if (w < 4) { cR(x - w / 2, y - h, w, max(1, h), col); return; }
    cR(x - w / 2, y - h * 6 / 10, w, h * 6 / 10, col);
    cR(x - w * 3 / 10, y - h, w * 6 / 10, h * 4 / 10 + 1, col);
    cR(x - w * 2 / 10, y - h + max(1, h / 10), w * 4 / 10, max(1, h * 2 / 10), RGB(40, 40, 70));
    cR(x - w / 2 + 1, y - h * 4 / 10, max(1, w / 6), max(1, h / 8), C_RED);
    cR(x + w / 2 - 1 - max(1, w / 6), y - h * 4 / 10, max(1, w / 6), max(1, h / 8), C_RED);
    cR(x - w / 2, y - max(1, h / 8), max(2, w / 5), max(1, h / 8), C_BG);
    cR(x + w / 2 - max(2, w / 5), y - max(1, h / 8), max(2, w / 5), max(1, h / 8), C_BG);
  }

  void render() {
    // Himmel im Abendrot
    const uint16_t sky[6] = {RGB(40, 10, 70), RGB(80, 20, 100), RGB(150, 30, 110), RGB(220, 60, 90), RGB(250, 120, 60), RGB(255, 180, 70)};
    for (int i = 0; i < 6; i++) cR(0, i * horizon / 6, SCR_W, horizon / 6 + 1, sky[i]);
    int sunX = SCR_W / 2 + (int)(sinf(bgX * 0.002f) * 30), sunY = horizon - 18;
    cC(sunX, sunY, 26, C_YELLOW);
    for (int i = 0; i < 4; i++) cR(sunX - 28, sunY + 2 + i * 6, 56, 2 + i / 2, sky[4]);
    // Berge (Parallaxe)
    for (int k = -1; k < SCR_W / 60 + 2; k++) {
      int bx = k * 60 - ((int)bgX % 60 + 60) % 60;
      int h = 18 + ((k + (int)(bgX / 60)) * 37 % 5 + 5) % 5 * 6;
      cTri(bx, horizon, bx + 30, horizon - h, bx + 60, horizon, RGB(60, 20, 80));
    }
    cR(0, horizon, SCR_W, SCR_H - horizon, RGB(30, 110, 40));

    // Strasse von nah nach fern projizieren (Puffer auf dem Stack)
    Proj proj[DRAW + 1];
    int base = (int)(z / SEGLEN);
    float frac = fmodf(z, SEGLEN) / SEGLEN;
    float camX = pX * ROADW;
    float x = 0, dx = -curveAt(base) * frac;
    for (int n = 0; n <= DRAW; n++) {
      float relZ = max(20.0f, (n + 1 - frac) * SEGLEN);
      float scale = DEPTH / relZ;
      x += dx;
      dx += curveAt(base + n);
      proj[n].x = (int)(SCR_W / 2 + scale * (x - camX) * SCR_W / 2);
      proj[n].y = (int)(horizon + scale * CAMH * SCR_H / 2);
      proj[n].w = (int)(scale * ROADW * SCR_W / 2);
    }
    for (int n = DRAW - 1; n >= 0; n--) {
      Proj& a = proj[n];        // nah
      Proj& b = proj[n + 1];    // fern
      if (a.y <= b.y) continue;
      int seg = base + n;
      bool alt = (seg / 3) % 2;
      cR(0, b.y, SCR_W, a.y - b.y + 1, alt ? RGB(30, 120, 45) : RGB(40, 150, 55));
      quad(a.x, a.y, a.w * 115 / 100, b.x, b.y, b.w * 115 / 100, alt ? C_WHITE : C_RED);
      quad(a.x, a.y, a.w, b.x, b.y, b.w, alt ? RGB(90, 90, 100) : RGB(100, 100, 110));
      if (alt) quad(a.x, a.y, max(1, a.w / 30), b.x, b.y, max(1, b.w / 30), C_WHITE);
    }
    // Palmen und Autos von hinten nach vorne
    for (int n = DRAW - 1; n >= 1; n--) {
      int seg = base + n;
      float s = (float)proj[n].w / (SCR_W / 2);
      if (seg % 7 == 0) {
        int side = (seg / 7) % 2 ? 1 : -1;
        drawPalm(proj[n].x + side * proj[n].w * 16 / 10, proj[n].y, s);
      }
      for (auto& c : cars) {
        int cs = (int)(c.z / SEGLEN);
        if (cs != seg) continue;
        drawCarSprite(proj[n].x + (int)(c.x * proj[n].w), proj[n].y, max(2, proj[n].w * 9 / 10), c.col);
      }
    }

    // Eigenes Auto
    int cw = landscape() ? 76 : 64, ch = cw * 45 / 100;
    int cx = SCR_W / 2 + (int)(steerVis * 6), cy = SCR_H - 6 - ((crashT / 3) % 2) - ((fabsf(pX) > 1 && tick % 2) ? 1 : 0);
    int tilt = (int)(steerVis * 4);
    cR(cx - cw / 2 + 4, cy - 8, 12, 8, C_BG);
    cR(cx + cw / 2 - 16, cy - 8, 12, 8, C_BG);
    cTri(cx - cw / 2, cy - 8, cx + cw / 2, cy - 8 + tilt, cx + cw / 2, cy - ch * 6 / 10, C_RED);
    cTri(cx - cw / 2, cy - 8, cx - cw / 2, cy - ch * 6 / 10 - tilt, cx + cw / 2, cy - ch * 6 / 10, C_RED);
    cR(cx - cw / 3, cy - ch, cw * 2 / 3, ch * 4 / 10, RGB(200, 30, 40));
    cR(cx - cw / 4, cy - ch + 3, cw / 2, ch * 2 / 10, RGB(40, 40, 80));
    cR(cx - cw / 2 + 3, cy - ch * 5 / 10, 12, 4, crashT ? C_YELLOW : C_ORANGE);
    cR(cx + cw / 2 - 15, cy - ch * 5 / 10, 12, 4, crashT ? C_YELLOW : C_ORANGE);
    cR(cx - 8, cy - ch * 4 / 10, 16, 4, C_GRAY);

    // Anzeigen
    char d[16];
    snprintf(d, sizeof(d), "%lu.%lu km", (unsigned long)(meters / 1000), (unsigned long)(meters % 1000 / 100));
    cTxt(d, 6, 4, 1, C_WHITE, textdatum_t::top_left);
    cNum("%ld P", (long)score, 6, 14, 1, C_GRAY);
    char t[12];
    snprintf(t, sizeof(t), "%d", (timeLeft + 29) / 30);
    cTxt("ZEIT", SCR_W / 2, 4, 1, C_YELLOW, textdatum_t::top_center);
    cTxt(t, SCR_W / 2, 14, 2, timeLeft < 300 && (tick / 5) % 2 ? C_RED : C_WHITE, textdatum_t::top_center);
    cNum("%ld km/h", (long)(spd / MAXSPD * 290), SCR_W - 6, 4, 1, C_CYAN, textdatum_t::top_right);
    cNum("HI %ld", (long)max(score, highscore[curGame]), SCR_W - 6, 14, 1, C_YELLOW, textdatum_t::top_right);
    if (msgT > 0) cTxt(msg, SCR_W / 2, horizon - 40, 2, C_WHITE);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
