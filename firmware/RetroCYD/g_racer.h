// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SUNSET RACE  -  Pseudo-3D-Rennen in den Sonnenuntergang
// ============================================================================
//  links/rechts lenken, A (oder hoch) = Gas, B (oder runter) = Bremse
//  Vor Ablauf der Zeit das naechste Tor (Checkpoint) erreichen: +25 Sekunden.
//  Neben der Strasse wird man langsam, andere Autos und Palmen bremsen hart.
//
//  Strecke mit Kurven und Huegeln, Dunst in der Ferne, Palmen, Schilder und
//  Plakatwaende am Rand. Alle Autos werden im selben Massstab gezeichnet wie
//  das eigene (das deshalb jetzt deutlich groesser ist).
// ============================================================================
#pragma once

namespace Racer {
  // --- Welt -----------------------------------------------------------------
  const float SEGLEN  = 200;          // Laenge eines Strassenstuecks
  const float ROADW   = 1300;         // halbe Strassenbreite
  const float CAMH    = 1000;         // Kamerahoehe ueber der Strasse
  const float DEPTH   = 0.84f;        // Brennweite
  const float PLAYERZ = 724;          // Abstand Kamera -> eigenes Auto
  const float CARW    = 780;          // Breite eines Autos (Welt)
  const float MAXSPD  = 560;          // Welt-Einheiten pro Spielschritt
  const int   DRAW    = 56;           // sichtbare Strassenstuecke
  const int   CHECKPOINT = 1200;      // Strassenstuecke zwischen zwei Toren
  const int   MAXSEG  = 1024;

  // Strecke: Laenge, Kurve (-6..6), Huegel (Hoehenaenderung ueber den Abschnitt)
  struct Section { int16_t len; int8_t curve; int16_t hill; };
  const Section TRACK[] = {
    {50, 0, 0},   {40, 2, 600},   {35, 4, 0},    {40, 0, -900},  {45, -3, 0},   {30, -5, 700},
    {40, 0, 1400},{30, 0, -1400}, {55, 3, 0},    {30, 0, 500},   {35, -6, -300},{45, -2, 0},
    {40, 0, 1100},{25, 0, -500},  {30, 5, -600}, {30, 2, 0},     {40, -4, 900}, {35, 0, -900},
    {50, 1, 0},   {30, -3, 1500}, {30, 0, -1500},{40, 4, 0},
  };
  const int NSEC = sizeof(TRACK) / sizeof(TRACK[0]);
  int trackLen;
  GArr<int16_t, MAXSEG> hgt;          // Hoehe am Anfang jedes Stuecks
  GArr<int8_t, MAXSEG>  crv;          // Kurve (x10) je Stueck, weich ein- und ausgeblendet

  struct Car { float z, x, spd; uint8_t col, kind; };
  const int NCARS = 8;
  Car cars[NCARS];

  struct Proj { int x, y, w; float s; };

  float z, pX, spd, bgX, steerVis, camY;
  int timeLeft, nextCheck, crashT, msgT, horizon, countT, checks, smokeT;
  uint32_t score, meters, bonus;
  bool braking;
  char msg[20];

  // --- Farben ---------------------------------------------------------------
  const uint16_t CAR_COLS[6] = {RGB(40, 90, 255), RGB(255, 210, 0), RGB(240, 240, 240),
                                RGB(40, 200, 90), RGB(170, 60, 230), RGB(255, 120, 20)};
  const uint16_t HAZE = RGB(250, 140, 110);

  uint16_t mix(uint16_t a, uint16_t b, int t) {        // t = 0..256 (0 = a)
    int ar = (a >> 11) & 31, ag = (a >> 5) & 63, ab = a & 31;
    int br = (b >> 11) & 31, bg = (b >> 5) & 63, bb = b & 31;
    return (uint16_t)(((ar + (br - ar) * t / 256) << 11) | ((ag + (bg - ag) * t / 256) << 5) | (ab + (bb - ab) * t / 256));
  }
  uint16_t shade(uint16_t c, int f) { return mix(RGB(0, 0, 0), c, f); }        // f = 0..256
  uint16_t light(uint16_t c, int f) { return mix(c, RGB(255, 255, 255), f); }

  inline uint32_t hash(uint32_t x) { x ^= x >> 16; x *= 0x7feb352d; x ^= x >> 15; x *= 0x846ca68b; x ^= x >> 16; return x; }

  // --- Strecke vorbereiten ----------------------------------------------------
  void buildTrack() {
    trackLen = 0;
    float h = 0;
    for (auto& s : TRACK) {
      for (int i = 0; i < s.len && trackLen < MAXSEG; i++) {
        float t = (float)i / s.len;
        float ease = (1 - cosf(t * PI)) / 2;               // Huegel weich
        float ramp = min(1.0f, min(t * 4, (1 - t) * 4));  // Kurve ein- und ausblenden
        hgt[trackLen] = (int16_t)(h + s.hill * ease);
        crv[trackLen] = (int8_t)(s.curve * 10 * ramp);
        trackLen++;
      }
      h += s.hill;
    }
    // Gesamtsteigung auf 0 bringen, damit die Runde nahtlos weitergeht
    for (int i = 0; i < trackLen; i++) hgt[i] = (int16_t)(hgt[i] - h * i / trackLen);
  }
  inline int segIdx(int seg) { seg %= trackLen; return seg < 0 ? seg + trackLen : seg; }
  inline float curveAt(int seg) { return crv[segIdx(seg)] / 10.0f; }
  inline float heightAt(float zz) {
    int s = (int)(zz / SEGLEN);
    float f = fmodf(zz, SEGLEN) / SEGLEN;
    float a = hgt[segIdx(s)], b = hgt[segIdx(s + 1)];
    return a + (b - a) * f;
  }

  void placeCar(Car& c, float ahead) {
    c.z = z + ahead;
    c.x = frand(-0.68f, 0.68f);
    c.spd = frand(0.28f, 0.6f) * MAXSPD;
    c.col = irand(0, 5);
    c.kind = irand(0, 3) == 0 ? 1 : 0;                   // 1 = Kleinbus
  }

  void start() {
    hgt.alloc(); crv.alloc();
    if (!hgt.p || !crv.p) { gameOver(0); return; }
    buildTrack();
    z = 0; pX = 0; spd = 0; bgX = 0; steerVis = 0;
    timeLeft = 50 * 30; nextCheck = CHECKPOINT; crashT = 0; score = 0; meters = 0; bonus = 0;
    checks = 0; smokeT = 0; braking = false;
    horizon = (int)(SCR_H * 0.44f);
    for (int i = 0; i < NCARS; i++) placeCar(cars[i], 3200 + i * 1500);
    countT = 3 * 30 + 15;                                // Startampel 3-2-1
    msg[0] = 0; msgT = 0;
  }

  // --- Spiellogik --------------------------------------------------------------
  void update() {
    if (msgT > 0) msgT--;
    if (crashT > 0) crashT--;
    if (smokeT > 0) smokeT--;
    if (countT > 0) {
      countT--;
      if (countT == 0) { snprintf(msg, sizeof(msg), "LOS!"); msgT = 30; rumble(150, 0x60); }
      return;
    }
    float sp = spd / MAXSPD;
    bool gas = cur.a || cur.up;
    braking = cur.b || cur.down;
    if (crashT == 0) {
      if (gas && !braking) spd += MAXSPD / (70 + sp * 100);   // oben heraus zieht er langsamer
      else if (braking) spd -= MAXSPD / 30;
      else spd -= MAXSPD / 280;
    }
    float steer = 0;
    if (cur.left) steer = -1;
    if (cur.right) steer = 1;
    if (abs(cur.ax) > 120 && !cur.left && !cur.right) steer = constrain(cur.ax / 400.0f, -1.0f, 1.0f);
    steerVis += (steer - steerVis) * 0.3f;

    int seg = (int)((z + PLAYERZ) / SEGLEN);
    float curve = curveAt(seg);
    pX += steer * (0.02f + 0.055f * sp) * (sp > 0.02f ? 1 : 0);
    pX -= curve * sp * sp * 0.0075f;                     // Fliehkraft
    bgX += curve * sp * 0.6f;

    // bergauf langsamer, bergab schneller
    float slope = (heightAt(z + PLAYERZ + SEGLEN) - heightAt(z + PLAYERZ)) / SEGLEN;
    spd -= slope * 6;

    if (fabsf(pX) > 1.0f) {                              // neben der Strasse
      if (spd > MAXSPD / 3) spd -= MAXSPD / 40;
      if (sp > 0.1f) smokeT = 6;
      if (tick % 8 == 0 && sp > 0.1f) rumble(60, 0x40);
      // Hindernisse am Rand (Palmen, Schilder, Plakate) stehen ab |x| ~ 1.45
      uint32_t hh = hash(segIdx(seg));
      int side = (hh & 1) ? 1 : -1;
      if (fabsf(pX) > 1.3f && (hh % 5) < 2 && side == (pX > 0 ? 1 : -1) && sp > 0.15f && crashT == 0) {
        spd *= 0.1f; crashT = 35; rumble(400, 0xD0); pX *= 0.6f;
      }
    }
    pX = constrain(pX, -2.0f, 2.0f);
    spd = constrain(spd, 0.0f, MAXSPD);
    z += spd;

    for (auto& c : cars) {
      c.z += c.spd;
      float rel = c.z - (z + PLAYERZ);
      if (rel < -2500 || rel > DRAW * SEGLEN * 1.3f) placeCar(c, DRAW * SEGLEN * frand(0.75f, 1.0f));
      if (crashT == 0 && rel > -80 && rel < 160 && fabsf(c.x - pX) < 0.44f) {
        spd = min(spd, c.spd * 0.5f);
        crashT = 28;
        c.z = z + PLAYERZ + 300;
        rumble(350, 0xC0);
      }
    }

    if (--timeLeft <= 0) { gameOver(score); return; }
    int dist = (int)(z / SEGLEN);
    meters = dist * 5;
    if (dist + (int)(PLAYERZ / SEGLEN) >= nextCheck) {
      nextCheck += CHECKPOINT;
      timeLeft += 25 * 30;
      bonus += 1000;
      checks++;
      snprintf(msg, sizeof(msg), "+25 SEK");
      msgT = 60;
      rumble(200, 0x70);
    }
    score = meters + bonus;
  }

  // --- Zeichnen ------------------------------------------------------------------
  void quad(int x1, int y1, int w1, int x2, int y2, int w2, uint16_t c) {
    if (y1 == y2) return;
    cTri(x1 - w1, y1, x1 + w1, y1, x2 + w2, y2, c);
    cTri(x1 - w1, y1, x2 + w2, y2, x2 - w2, y2, c);
  }
  // Viereck zwischen zwei x-Bereichen (fuer Randstreifen und Fahrbahnlinien)
  void band(int y1, int l1, int r1, int y2, int l2, int r2, uint16_t c) {
    if (y1 == y2) return;
    cTri(l1, y1, r1, y1, r2, y2, c);
    cTri(l1, y1, r2, y2, l2, y2, c);
  }

  void drawSky() {
    // Verlauf mit versetzten Zwischenzeilen (gegen harte Farbstufen)
    const uint16_t sky[8] = {RGB(30, 10, 60), RGB(55, 15, 85), RGB(95, 20, 105), RGB(145, 30, 115),
                             RGB(200, 50, 110), RGB(235, 85, 90), RGB(250, 130, 70), RGB(255, 175, 80)};
    int bh = (horizon + 7) / 8;
    for (int i = 0; i < 8; i++) {
      int y = i * bh;
      cR(0, y, SCR_W, bh, sky[i]);
      if (i < 7) for (int k = 0; k < 3; k++) cR(0, y + bh - 1 - k * 2, SCR_W, 1, sky[i + 1]);
    }
    // Sonne mit Streifen
    int sunX = SCR_W / 2 + (int)(sinf(bgX * 0.0015f) * 40), sunR = SCR_H / 7, sunY = horizon - sunR / 3;
    cC(sunX, sunY, sunR + 3, RGB(255, 150, 80));
    cC(sunX, sunY, sunR, RGB(255, 225, 60));
    cC(sunX, sunY - sunR / 3, sunR * 2 / 3, RGB(255, 245, 140));
    for (int i = 0; i < 6; i++) {
      int sy = sunY + i * sunR / 6;
      cR(sunX - sunR - 2, sy, 2 * sunR + 4, 1 + i / 2, sky[6]);
    }
    // Wolkenbaender
    for (int k = 0; k < 4; k++) {
      int cx = ((k * 97 - (int)(bgX * 0.25f)) % (SCR_W + 120) + SCR_W + 120) % (SCR_W + 120) - 60;
      int cy = horizon / 4 + k * horizon / 7;
      cE(cx, cy, 34 + k * 6, 3, RGB(120, 40, 110));
      cE(cx + 20, cy + 3, 22, 2, RGB(170, 60, 110));
    }
    // ferne Stadt (langsame Parallaxe) und Berge
    int off = ((int)(bgX * 0.4f) % 26 + 26) % 26;
    for (int k = -1; k < SCR_W / 26 + 2; k++) {
      int bx = k * 26 - off;
      int idx = k + (int)(bgX * 0.4f) / 26;
      uint32_t hh = hash(idx + 1000);
      int bhgt = 8 + hh % 22;
      cR(bx, horizon - bhgt, 18 + (hh >> 8) % 7, bhgt, RGB(70, 25, 90));
      if ((hh >> 12) % 3 == 0) cP(bx + 4, horizon - bhgt + 4, RGB(255, 220, 120));
      if ((hh >> 14) % 3 == 0) cP(bx + 9, horizon - bhgt + 8, RGB(255, 220, 120));
    }
    int moff = ((int)bgX % 80 + 80) % 80;
    for (int k = -1; k < SCR_W / 80 + 2; k++) {
      int bx = k * 80 - moff;
      int idx = k + (int)bgX / 80;
      int h = 14 + (int)(hash(idx) % 4) * 6;
      cTri(bx, horizon, bx + 40, horizon - h, bx + 80, horizon, RGB(45, 15, 70));
      cTri(bx + 40, horizon - h, bx + 52, horizon - h + 6, bx + 46, horizon, RGB(60, 22, 85));
    }
  }

  void drawPalm(int x, int y, float s, uint16_t fog, int t) {
    int h = (int)(s * 2600);                               // s = Pixel pro Welt-Einheit
    if (h < 4) return;
    int w = max(1, (int)(s * 170));
    uint16_t trunk = mix(RGB(120, 70, 40), fog, t), leaf = mix(RGB(20, 120, 50), fog, t), leaf2 = mix(RGB(40, 170, 70), fog, t);
    // leicht gebogener Stamm aus drei Stuecken
    int bend = (int)(s * 260);
    cR(x - w / 2, y - h / 3, w, h / 3 + 1, trunk);
    cR(x - w / 2 + bend / 3, y - h * 2 / 3, w, h / 3 + 1, trunk);
    cR(x - w / 2 + bend * 2 / 3, y - h, w, h / 3 + 1, trunk);
    int tx = x + bend * 2 / 3, ty = y - h;
    int r = max(3, (int)(s * 720));
    cTri(tx, ty, tx - r, ty + r / 2, tx - r / 2, ty + r / 5, leaf);
    cTri(tx, ty, tx + r, ty + r / 2, tx + r / 2, ty + r / 5, leaf);
    cTri(tx, ty - 1, tx - r * 3 / 4, ty - r / 3, tx - r / 5, ty + r / 6, leaf2);
    cTri(tx, ty - 1, tx + r * 3 / 4, ty - r / 3, tx + r / 5, ty + r / 6, leaf2);
    cTri(tx, ty, tx - r * 6 / 5, ty + r, tx - r * 2 / 3, ty + r / 3, leaf);         // haengende Wedel
    cTri(tx, ty, tx + r * 6 / 5, ty + r, tx + r * 2 / 3, ty + r / 3, leaf);
    cTri(tx, ty - 1, tx - r / 6, ty - r * 2 / 3, tx + r / 4, ty - r / 2, leaf2);
    cC(tx, ty, max(1, r / 6), mix(RGB(90, 60, 30), fog, t));
  }

  void drawSign(int x, int y, float s, int dir, uint16_t fog, int t) {
    int w = (int)(s * 900), h = (int)(s * 650);
    if (w < 4) return;
    int py = y - (int)(s * 1100);
    cR(x - max(1, w / 20), py, max(1, w / 10), y - py, mix(RGB(150, 150, 160), fog, t));
    cR(x - w / 2, py - h, w, h, mix(RGB(250, 250, 250), fog, t));
    cR(x - w / 2 + max(1, w / 16), py - h + max(1, h / 10), w - max(2, w / 8), h - max(2, h / 5), mix(RGB(220, 30, 30), fog, t));
    uint16_t a = mix(RGB(255, 255, 255), fog, t);
    for (int k = -1; k <= 1; k++) {                      // Pfeile in Kurvenrichtung
      int ax = x + k * w / 4, ay = py - h / 2, aw = max(2, w / 8);
      cTri(ax + dir * aw, ay, ax - dir * aw / 2, ay - h / 4, ax - dir * aw / 2, ay + h / 4, a);
    }
  }

  void drawBoard(int x, int y, float s, uint32_t hh, uint16_t fog, int t) {
    int w = (int)(s * 2600), h = (int)(s * 1050);
    if (w < 6) return;
    int py = y - (int)(s * 1300);
    uint16_t post = mix(RGB(80, 70, 70), fog, t);
    cR(x - w / 3, py, max(1, w / 30), y - py, post);
    cR(x + w / 3, py, max(1, w / 30), y - py, post);
    const uint16_t bgc[3] = {RGB(30, 30, 90), RGB(240, 200, 30), RGB(200, 30, 120)};
    const uint16_t fgc[3] = {RGB(0, 230, 240), RGB(200, 40, 40), RGB(255, 240, 240)};
    int k = hh % 3;
    cR(x - w / 2, py - h, w, h, mix(bgc[k], fog, t));
    cR(x - w / 2, py - h, w, max(1, h / 10), mix(RGB(255, 255, 255), fog, t));
    if (w > 40) {
      static const char* txts[3] = {"SUNSET", "TURBO", "RETRO"};
      float ts = max(1.0f, (float)w / 60);
      cTxt(txts[k], x, py - h / 2, ts, mix(fgc[k], fog, t));
    } else {
      cR(x - w / 3, py - h / 2 - max(1, h / 8), w * 2 / 3, max(1, h / 4), mix(fgc[k], fog, t));
    }
  }

  // Auto von hinten. x/y = Mitte unten, w = Breite in Pixeln
  void drawCar(int x, int y, int w, uint16_t body, bool brake, float lean, uint8_t kind, uint16_t fog, int t) {
    if (w < 3) return;
    int h = w * 52 / 100;
    if (kind == 1) h = w * 70 / 100;                     // Kleinbus: hoeher
    uint16_t b = mix(body, fog, t), dark = mix(shade(body, 150), fog, t), hi = mix(light(body, 90), fog, t);
    uint16_t blk = mix(RGB(15, 15, 20), fog, t), glass = mix(RGB(40, 50, 90), fog, t);
    if (w < 12) {                                        // sehr weit weg: nur Umriss
      cR(x - w / 2, y - h * 6 / 10, w, h * 6 / 10, b);
      cR(x - w * 3 / 10, y - h, w * 6 / 10, h * 4 / 10 + 1, dark);
      return;
    }
    int lx = (int)(lean * w / 22);                        // Neigung in der Kurve
    cE(x, y - 1, w * 55 / 100, max(1, w / 18), mix(RGB(10, 30, 15), fog, t));          // Schatten
    // Reifen
    int tw = w * 17 / 100, th = h * 32 / 100;
    cRR(x - w / 2 + w / 40, y - th, tw, th, max(1, tw / 4), blk);
    cRR(x + w / 2 - w / 40 - tw, y - th, tw, th, max(1, tw / 4), blk);
    // Karosserie
    int bodyTop = y - h * 72 / 100, bodyBot = y - h * 22 / 100;
    cRR(x - w / 2, bodyTop, w, bodyBot - bodyTop, max(1, w / 16), b);
    cR(x - w / 2 + w / 30, bodyTop, w - w / 15, max(1, h / 18), hi);                   // Glanzkante
    cR(x - w * 47 / 100, bodyBot - h * 6 / 100, w * 94 / 100, h * 12 / 100, dark);      // Stossstange
    // Aufbau mit Heckscheibe
    int cabTop = y - h, cabBot = bodyTop + 1;
    int cbw = (kind == 1 ? w * 86 / 100 : w * 74 / 100) / 2, ctw = (kind == 1 ? w * 80 / 100 : w * 56 / 100) / 2;
    band(cabBot, x - cbw, x + cbw, cabTop, x - ctw + lx, x + ctw + lx, dark);
    int ins = max(1, w / 28);
    band(cabBot - ins, x - cbw + ins * 2, x + cbw - ins * 2, cabTop + ins, x - ctw + ins + lx, x + ctw - ins + lx, glass);
    if (w > 30) cL(x - ctw / 2 + lx, cabTop + ins + 1, x - ctw / 2 - cbw / 3, cabBot - ins - 1, mix(RGB(120, 140, 200), fog, t));
    // Rueckleuchten-Band, Leuchten, Kennzeichen
    int ly = bodyTop + h * 14 / 100, lh = max(2, h * 14 / 100);
    cR(x - w * 46 / 100, ly, w * 92 / 100, lh, blk);
    uint16_t lamp = brake ? RGB(255, 60, 60) : RGB(170, 20, 30);
    cR(x - w * 44 / 100, ly + 1, w * 26 / 100, max(1, lh - 2), mix(lamp, fog, t));
    cR(x + w * 18 / 100, ly + 1, w * 26 / 100, max(1, lh - 2), mix(lamp, fog, t));
    if (w > 40) {
      cR(x - w * 44 / 100 + w / 40, ly + 2, w / 14, max(1, lh - 4), mix(RGB(255, 170, 40), fog, t));
      cR(x + w * 44 / 100 - w / 40 - w / 14, ly + 2, w / 14, max(1, lh - 4), mix(RGB(255, 170, 40), fog, t));
      if (brake) { cR(x - w * 44 / 100, ly - 2, w * 26 / 100, 2, RGB(255, 140, 140)); cR(x + w * 18 / 100, ly - 2, w * 26 / 100, 2, RGB(255, 140, 140)); }
      cR(x - w * 9 / 100, bodyBot - h * 18 / 100, w * 18 / 100, max(2, h * 10 / 100), mix(RGB(240, 240, 230), fog, t));
      cR(x - w * 7 / 100, bodyBot - h * 15 / 100, w * 14 / 100, 1, mix(RGB(60, 60, 60), fog, t));
      // Auspuff
      cC(x - w * 30 / 100, bodyBot + 1, max(1, w / 40), mix(RGB(110, 110, 120), fog, t));
      cC(x - w * 22 / 100, bodyBot + 1, max(1, w / 40), mix(RGB(110, 110, 120), fog, t));
    }
    if (kind == 0 && w > 24) {                            // Heckfluegel
      int sy = cabTop + (cabBot - cabTop) * 6 / 10;
      cR(x - w * 45 / 100 + lx / 2, sy, w * 90 / 100, max(1, h / 14), blk);
      cR(x - w * 30 / 100 + lx / 2, sy, max(1, w / 30), bodyTop - sy, blk);
      cR(x + w * 30 / 100 + lx / 2, sy, max(1, w / 30), bodyTop - sy, blk);
    }
  }

  void render() {
    drawSky();
    cR(0, horizon, SCR_W, SCR_H - horizon, mix(RGB(40, 140, 55), HAZE, 200));

    // Strasse projizieren (Huegel: Hoehe je Stueck)
    Proj proj[DRAW + 1];
    int base = (int)(z / SEGLEN);
    float frac = fmodf(z, SEGLEN) / SEGLEN;
    camY = heightAt(z + PLAYERZ) + CAMH;
    float camX = pX * ROADW;
    float x = 0, dx = -curveAt(base) * frac;
    for (int n = 0; n <= DRAW; n++) {
      float relZ = max(30.0f, (n + 1 - frac) * SEGLEN);
      float s = DEPTH / relZ;
      x += dx;
      dx += curveAt(base + n);
      float wy = hgt[segIdx(base + n + 1)];
      proj[n].x = (int)(SCR_W / 2 + s * (x - camX) * SCR_W / 2);
      proj[n].y = (int)(horizon + s * (camY - wy) * SCR_H / 2);
      proj[n].w = (int)(s * ROADW * SCR_W / 2);
      proj[n].s = s;
    }

    // von hinten nach vorne: Strassenstueck, dann Dinge darauf (Huegel verdecken richtig)
    int playerSeg = (int)((z + PLAYERZ) / SEGLEN);
    for (int n = DRAW - 1; n >= 0; n--) {
      Proj& a = proj[n];
      Proj& b = proj[n + 1];
      int seg = base + n;
      int fog = n > DRAW / 3 ? (n - DRAW / 3) * 230 / (DRAW - DRAW / 3) : 0;
      if (a.y > b.y) {
        bool alt = (segIdx(seg) / 3) % 2;
        uint16_t grass = mix(alt ? RGB(40, 140, 55) : RGB(55, 165, 65), HAZE, fog);
        uint16_t kerb  = mix(alt ? RGB(240, 240, 240) : RGB(220, 30, 30), HAZE, fog);
        uint16_t road  = mix(alt ? RGB(85, 80, 95) : RGB(95, 90, 105), HAZE, fog);
        uint16_t line  = mix(RGB(240, 240, 230), HAZE, fog);
        cR(0, b.y, SCR_W, a.y - b.y + 1, grass);
        quad(a.x, a.y, a.w * 112 / 100, b.x, b.y, b.w * 112 / 100, kerb);
        quad(a.x, a.y, a.w, b.x, b.y, b.w, road);
        if (alt) {                                      // zwei gestrichelte Fahrbahnlinien
          for (int k = -1; k <= 1; k += 2) {
            int ax = a.x + k * a.w / 3, bx2 = b.x + k * b.w / 3;
            int aw = max(1, a.w / 40), bw = max(1, b.w / 40);
            band(a.y, ax - aw, ax + aw, b.y, bx2 - bw, bx2 + bw, line);
          }
        }
        if (segIdx(seg) == segIdx(nextCheck) || segIdx(seg) == segIdx(nextCheck + 1))   // Startlinie am Tor
          quad(a.x, a.y, a.w, b.x, b.y, b.w, mix((seg & 1) ? RGB(240, 240, 240) : RGB(20, 20, 20), HAZE, fog));
      }
      if (n < 3) continue;                              // Dinge zwischen Kamera und eigenem Auto nicht zeichnen
      const bool near = n < 6;                          // ganz nah: Randobjekte weglassen (wuerden das Bild fuellen)
      // Rand: Palmen, Kurvenschilder, Plakate
      uint32_t hh = hash(segIdx(seg));
      int side = (hh & 1) ? 1 : -1;
      float cv = curveAt(seg);
      int ox = a.x + side * a.w * 150 / 100;
      const float sw = a.s * SCR_W / 2;                // Pixel pro Welt-Einheit auf diesem Stueck
      if (near) {
      } else if (fabsf(cv) >= 3 && (segIdx(seg) % 4) == 0) {
        int sd = cv > 0 ? -1 : 1;                       // Schild aussen
        drawSign(a.x + sd * a.w * 135 / 100, a.y, sw, cv > 0 ? 1 : -1, HAZE, fog);
      } else if ((hh % 5) < 2) {
        if ((hh >> 4) % 6 == 0) drawBoard(ox + side * a.w / 4, a.y, sw, hh >> 8, HAZE, fog);
        else drawPalm(ox, a.y, sw, HAZE, fog);
      }
      // Tor (Checkpoint)
      if (segIdx(seg) == segIdx(nextCheck)) {
        int pw = max(1, a.w / 30), ph = (int)(sw * 2400);
        uint16_t pc = mix(RGB(230, 230, 240), HAZE, fog);
        cR(a.x - a.w * 115 / 100, a.y - ph, pw, ph, pc);
        cR(a.x + a.w * 115 / 100 - pw, a.y - ph, pw, ph, pc);
        int bh = max(3, ph / 4);
        cR(a.x - a.w * 115 / 100, a.y - ph - bh, a.w * 230 / 100, bh, mix(RGB(20, 20, 60), HAZE, fog));
        cR(a.x - a.w * 115 / 100, a.y - ph - bh, a.w * 230 / 100, max(1, bh / 6), mix(RGB(255, 210, 0), HAZE, fog));
        if (bh > 9) cTxt("CHECKPOINT", a.x, a.y - ph - bh / 2, max(1.0f, bh / 12.0f), mix(RGB(255, 220, 0), HAZE, fog));
      }
      // andere Autos auf diesem Stueck
      for (auto& c : cars) {
        if ((int)(c.z / SEGLEN) != seg) continue;
        if (seg <= playerSeg) continue;
        int cw = (int)(a.s * CARW * SCR_W / 2);
        drawCar(a.x + (int)(c.x * a.w), a.y, cw, CAR_COLS[c.col], false, 0, c.kind, HAZE, fog);
      }
    }

    // Eigenes Auto (gleicher Massstab wie die anderen)
    float ps = DEPTH / PLAYERZ;
    int cw = (int)(ps * CARW * SCR_W / 2);
    int roadY = (int)(horizon + ps * CAMH * SCR_H / 2);
    int cx = SCR_W / 2 + (int)(steerVis * 6);
    int cy = min(SCR_H - 2, roadY) - ((crashT / 3) % 2) * 2 - ((fabsf(pX) > 1 && tick % 2) ? 1 : 0);
    if (smokeT > 0 || (crashT > 0 && spd > 50)) {         // Staub/Rauch
      for (int k = 0; k < 4; k++) {
        int r = 4 + (int)((tick * 3 + k * 7) % 9);
        int sx = cx + (k % 2 ? 1 : -1) * (cw / 2 - 6) + irand(-4, 4);
        cC(sx, cy - 4 - k * 3, r, k % 2 ? RGB(170, 150, 120) : RGB(130, 115, 95));
      }
    }
    drawCar(cx, cy, cw, RGB(220, 30, 40), braking || crashT > 0, steerVis, 0, HAZE, 0);
    if (spd > MAXSPD * 0.9f && (tick % 4) < 2 && !braking) {   // Auspuff-Flammen bei Vollgas
      int fy = cy - cw * 52 / 100 * 22 / 100 + 1;
      cC(cx - cw * 30 / 100, fy + 3, 3, RGB(255, 160, 40));
      cC(cx - cw * 22 / 100, fy + 3, 3, RGB(255, 200, 60));
    }

    // --- Anzeigen ---
    char t[16];
    const int pw = min(92, (SCR_W - 64) / 2 - 4);       // dunkle Felder hinter den Anzeigen (passt auch hochkant)
    cRR(2, 2, pw, 38, 6, RGB(20, 10, 40));
    cRR(SCR_W - 2 - pw, 2, pw, 38, 6, RGB(20, 10, 40));
    // Zeit (Mitte oben, eingerahmt)
    snprintf(t, sizeof(t), "%d", (timeLeft + 29) / 30);
    bool warn = timeLeft < 300 && (tick / 5) % 2;
    cRR(SCR_W / 2 - 30, 2, 60, 34, 6, RGB(20, 10, 40));
    cTxt("ZEIT", SCR_W / 2, 5, 1, C_YELLOW, textdatum_t::top_center);
    cTxt(t, SCR_W / 2, 15, 2.5f, warn ? C_RED : C_WHITE, textdatum_t::top_center);
    // links: Strecke, Punkte, Tore
    snprintf(t, sizeof(t), "%lu.%lu km", (unsigned long)(meters / 1000), (unsigned long)(meters % 1000 / 100));
    cTxt(t, 8, 5, 1.5f, C_WHITE, textdatum_t::top_left);
    cNum("%ld P", (long)score, 8, 19, 1, RGB(200, 200, 220));
    cNum("TOR %ld", (long)checks, 8, 29, 1, C_CYAN);
    // rechts: Tempo mit Balken, Rekord
    int kmh = (int)(spd / MAXSPD * 290);
    cNum("%ld", (long)kmh, SCR_W - 38, 5, 2, C_WHITE, textdatum_t::top_right);
    cTxt("km/h", SCR_W - 8, 11, 1, C_CYAN, textdatum_t::top_right);
    int bw = pw - 12, bx = SCR_W - 8 - bw;
    cR(bx, 22, bw, 5, RGB(30, 20, 50));
    int fill = bw * kmh / 290;
    for (int i = 0; i < fill; i += 4) cR(bx + i, 22, 3, 5, i < bw * 6 / 10 ? C_GREEN : (i < bw * 85 / 100 ? C_YELLOW : C_RED));
    cNum("HI %ld", (long)max(score, highscore[curGame]), SCR_W - 8, 30, 1, C_YELLOW, textdatum_t::top_right);

    // Startampel und Meldungen
    if (countT > 0) {
      int n = (countT + 29) / 30;
      snprintf(t, sizeof(t), "%d", min(3, n));
      cC(SCR_W / 2, horizon - 10, 22, RGB(20, 10, 40));
      cTxt(t, SCR_W / 2, horizon - 9, 3, n > 1 ? C_RED : C_YELLOW);
    } else if (msgT > 0 && msg[0]) {
      cTxt(msg, SCR_W / 2 + 2, horizon - 18, 3, RGB(60, 20, 60));
      cTxt(msg, SCR_W / 2, horizon - 20, 3, (msgT / 4) % 2 ? C_YELLOW : C_WHITE);
    }
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
