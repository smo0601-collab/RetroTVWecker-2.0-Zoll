#pragma once
/*
  face_easter.h - Zifferblatt "Ostern"
  Fruehlingswiese unter blauem Himmel: Ein Osterhase (eigene Figur) hoppelt hin und her, setzt sich,
  wackelt mit den Ohren. Im Gras liegen bunt bemalte Eier versteckt, Schmetterlinge flattern, Wolken
  ziehen. Die Ziffern der Uhrzeit sind pastellfarbene Ostereier-Farben. Rechts der Countdown bis
  Ostersonntag (nach dem echten Osterdatum berechnet), an Ostersonntag und -montag "FROHE OSTERN".
  Zur vollen Minute schluepft im Nest ein Kueken und ruft "PIEP!".
  Uebergang: Bunte Eier purzeln von oben herein.
*/
#include "faces_fx.h"

namespace f_egg {
using namespace fc;

enum : uint8_t { O_SKY1 = 1, O_SKY2, O_SKY3, O_SUN, O_SUN2, O_CLOUD, O_HILL, O_HILL2, O_GRASS, O_GRASS2,
                 O_FUR, O_FUR2, O_BELLY, O_PINK, O_EYE, O_WHITE, O_NEST, O_NEST2, O_SHELL, O_CHICK, O_BEAK,
                 O_E1, O_E2, O_E3, O_E4, O_E5, O_FL1, O_FL2, O_FLC, O_BF1, O_BF2,
                 O_D1, O_D2, O_D3, O_D4, O_SH, O_TXT, O_DIM, O_ACC, O_GTXT };

static uint32_t lastT, stT, hatchT;
static float    bx, dirF;
static uint8_t  mode, hops, lastMm, digCol;

static void enter(uint32_t now) {
  pal(O_SKY1, 120, 190, 250); pal(O_SKY2, 150, 208, 252); pal(O_SKY3, 186, 226, 255);
  pal(O_SUN, 255, 230, 90); pal(O_SUN2, 255, 200, 60); pal(O_CLOUD, 252, 252, 255);
  pal(O_HILL, 110, 200, 90); pal(O_HILL2, 80, 170, 70); pal(O_GRASS, 50, 140, 50); pal(O_GRASS2, 140, 220, 100);
  pal(O_FUR, 196, 160, 120); pal(O_FUR2, 150, 116, 84); pal(O_BELLY, 240, 226, 206); pal(O_PINK, 255, 150, 170);
  pal(O_EYE, 30, 20, 20); pal(O_WHITE, 255, 255, 255);
  pal(O_NEST, 150, 104, 50); pal(O_NEST2, 110, 72, 30); pal(O_SHELL, 250, 246, 236); pal(O_CHICK, 255, 224, 60); pal(O_BEAK, 255, 140, 30);
  pal(O_E1, 255, 120, 140); pal(O_E2, 120, 170, 255); pal(O_E3, 255, 210, 70); pal(O_E4, 150, 220, 120); pal(O_E5, 200, 140, 255);
  pal(O_FL1, 255, 90, 110); pal(O_FL2, 255, 240, 120); pal(O_FLC, 255, 180, 40);
  pal(O_BF1, 255, 150, 40); pal(O_BF2, 120, 120, 255);
  pal(O_D1, 240, 90, 150); pal(O_D2, 245, 170, 20); pal(O_D3, 40, 170, 90); pal(O_D4, 140, 90, 235);
  pal(O_SH, 255, 255, 255); pal(O_TXT, 40, 70, 130); pal(O_DIM, 70, 100, 150); pal(O_ACC, 230, 70, 120); pal(O_GTXT, 20, 80, 30);
  lastT = now; stT = now; hatchT = 0; bx = 70.0f; dirF = 1.0f; mode = 0; hops = 3; lastMm = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, O_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, digCol); }

// Ei mit Muster: pat 0 Streifen, 1 Punkte, 2 Zickzack
static void egg(int cx, int cy, uint8_t col, uint8_t col2, int pat) {
  fb->fillCircle(cx, cy + 1, 3, col);
  fb->fillRect(cx - 2, cy - 3, 5, 4, col);
  fb->drawFastHLine(cx - 1, cy - 4, 3, col);
  if (pat == 0) { fb->drawFastHLine(cx - 2, cy - 1, 5, col2); fb->drawFastHLine(cx - 3, cy + 2, 7, col2); }
  else if (pat == 1) { fb->drawPixel(cx - 1, cy - 2, col2); fb->drawPixel(cx + 1, cy, col2); fb->drawPixel(cx - 2, cy + 2, col2); fb->drawPixel(cx + 2, cy + 3, col2); }
  else for (int k = -3; k <= 3; k++) fb->drawPixel(cx + k, cy + ((k & 1) ? 0 : 1), col2);
  fb->drawPixel(cx - 1, cy - 2, O_WHITE);
}

// gespiegeltes Rechteck relativ zur Hasenmitte
static void R(int ox, int y, int w, int h, uint8_t col, int d) {
  fb->fillRect(d > 0 ? (int)bx + ox : (int)bx - ox - w + 1, y, w, h, col);
}
static void P(int ox, int y, uint8_t col, int d) { fb->drawPixel(d > 0 ? (int)bx + ox : (int)bx - ox, y, col); }

static void bunny(int by, int d, bool twitch, bool stretch) {
  // Schwanz
  R(-11, by - 11, 4, 4, O_WHITE, d);
  // Koerper
  R(-8, by - 13, 15, 10, O_FUR, d);
  R(-9, by - 11, 17, 7, O_FUR, d);
  R(-5, by - 9, 9, 5, O_BELLY, d);
  R(-8, by - 4, 15, 1, O_FUR2, d);
  // Fuesse
  if (stretch) { R(-14, by - 5, 7, 2, O_FUR2, d); R(7, by - 4, 4, 2, O_FUR2, d); }
  else { R(-9, by - 3, 8, 3, O_FUR2, d); R(4, by - 3, 4, 3, O_FUR, d); }
  // Kopf
  R(4, by - 21, 10, 9, O_FUR, d);
  R(5, by - 22, 8, 11, O_FUR, d);
  R(10, by - 16, 4, 4, O_BELLY, d);
  P(9, by - 18, O_EYE, d); P(10, by - 18, O_EYE, d); P(9, by - 19, O_WHITE, d);
  P(13, by - 16, O_PINK, d); P(14, by - 15, O_PINK, d);
  P(15, by - 14, O_FUR2, d); P(15, by - 16, O_FUR2, d);       // Schnurrhaare
  // Ohren
  R(5, by - 33, 3, 12, O_FUR, d); R(6, by - 31, 1, 9, O_PINK, d);
  if (twitch) { R(9, by - 26, 3, 5, O_FUR, d); R(11, by - 28, 4, 3, O_FUR, d); }
  else { R(9, by - 32, 3, 11, O_FUR, d); R(10, by - 30, 1, 8, O_PINK, d); }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  const float t = secs(now, stT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) hatchT = now;
    lastMm = c.mm;
  }

  // ---- Himmel, Sonne, Wolken ----
  fb->fillRect(0, 0, FB_W, 30, O_SKY1);
  fb->fillRect(0, 30, FB_W, 25, O_SKY2);
  fb->fillRect(0, 55, FB_W, 25, O_SKY3);
  const float ra = (float)(now % 12000) / 12000.0f * 6.2832f;
  for (int k = 0; k < 8; k++) {
    const float a = ra + k * 0.785f;
    fb->drawLine(16 + (int)(cosf(a) * 12.0f), 16 + (int)(sinf(a) * 12.0f), 16 + (int)(cosf(a) * 16.0f), 16 + (int)(sinf(a) * 16.0f), O_SUN2);
  }
  fb->fillCircle(16, 16, 9, O_SUN);
  for (int i = 0; i < 2; i++) {
    const int x = (int)((now / (140 + i * 90) + (uint32_t)i * 80) % 200) - 30, y = 42 + i * 16;
    fb->fillCircle(x, y, 5, O_CLOUD); fb->fillCircle(x + 7, y - 3, 6, O_CLOUD); fb->fillCircle(x + 14, y, 5, O_CLOUD);
    fb->fillRect(x, y, 15, 5, O_CLOUD);
  }

  // ---- Wiese ----
  fb->fillCircle(40, 190, 116, O_HILL2);
  fb->fillCircle(140, 200, 120, O_HILL);
  fb->fillRect(0, 96, FB_W, 24, O_HILL);
  for (int x = 1; x < FB_W; x += 3) {
    const int h = 2 + (x * 7) % 4;
    fb->drawFastVLine(x, 120 - h - (x * 13) % 6, h, (x % 2) ? O_GRASS : O_GRASS2);
  }

  // Blumen
  const int fx[6] = {44, 58, 100, 112, 150, 6}, fy[6] = {88, 98, 92, 104, 90, 80};
  for (int i = 0; i < 6; i++) {
    fb->drawFastVLine(fx[i], fy[i], 6, O_GRASS);
    const uint8_t pc = (i & 1) ? O_FL2 : O_FL1;
    fb->drawPixel(fx[i] - 1, fy[i], pc); fb->drawPixel(fx[i] + 1, fy[i], pc);
    fb->drawPixel(fx[i], fy[i] - 1, pc); fb->drawPixel(fx[i], fy[i] + 1, pc);
    fb->drawPixel(fx[i], fy[i], O_FLC);
  }

  // versteckte Eier (halb im Gras)
  const int ex[5] = {66, 92, 124, 140, 80}, ey[5] = {100, 95, 102, 96, 108};
  const uint8_t ec[5] = {O_E1, O_E2, O_E3, O_E4, O_E5}, ec2[5] = {O_E3, O_WHITE, O_E1, O_E5, O_E3};
  for (int i = 0; i < 5; i++) {
    egg(ex[i], ey[i], ec[i], ec2[i], i % 3);
    for (int k = -4; k <= 4; k += 2) fb->drawFastVLine(ex[i] + k, ey[i] + 2 - (k & 2), 3, O_GRASS);
  }

  // ---- Nest mit Kueken ----
  const float ht = hatchT ? secs(now, hatchT) : 99.0f;
  egg(14, 95, O_E2, O_WHITE, 1);
  egg(30, 95, O_E1, O_E3, 0);
  if (ht < 1.0f) {                                               // Ei wackelt und bekommt Risse
    const int wob = ((now / 60) & 1) ? 1 : -1;
    egg(22 + wob, 94, O_SHELL, O_SHELL, 1);
    for (int k = -3; k <= 3; k++) fb->drawPixel(22 + wob + k, 93 + ((k & 1) ? 0 : 1), O_NEST2);
  } else if (ht < 5.0f) {
    fb->fillCircle(22, 95, 3, O_SHELL);                         // untere Schale
    fb->fillCircle(22, 90, 4, O_CHICK);                          // Kueken
    fb->drawPixel(21, 89, O_EYE); fb->drawPixel(24, 89, O_EYE);
    fb->fillTriangle(22, 91, 24, 91, 23, 93, O_BEAK);
    fb->drawFastHLine(19, 85, 6, O_SHELL); fb->drawFastHLine(20, 84, 4, O_SHELL);     // Schalenhut
    fb->fillRoundRect(28, 76, 36, 11, 3, O_WHITE);
    fb->fillTriangle(28, 84, 32, 84, 25, 88, O_WHITE);
    txt(32, 78, TR(D_PEEP), O_ACC);
  } else egg(22, 94, O_SHELL, O_E4, 2);
  fb->fillRoundRect(6, 96, 32, 7, 3, O_NEST);
  for (int x = 8; x < 36; x += 3) fb->drawLine(x, 97, x + 3, 101, O_NEST2);
  fb->drawFastHLine(6, 99, 32, O_NEST2);

  // ---- Osterhase ----
  const int D = dirF > 0 ? 1 : -1;
  int by = 104;
  bool stretch = false, twitch = false;
  if (mode == 0) {                                                // huepfen
    const float a = t / 0.45f;
    if (a >= 1.0f) {
      stT = now;
      if (--hops == 0) { mode = 1; hops = 2 + (uint8_t)rn(4); }
    } else {
      bx += dirF * 30.0f * dt;
      by -= (int)(sinf(a * 3.1416f) * 9.0f);
      stretch = a > 0.15f && a < 0.85f;
    }
    if (bx > 142.0f) { bx = 142.0f; dirF = -1.0f; }
    if (bx < 52.0f)  { bx = 52.0f; dirF = 1.0f; }
  } else {                                                        // sitzen
    twitch = ((now / 350) % 4) == 0;
    if (t > 3.0f + (float)(rn(100) == 0 ? 2 : 0)) { mode = 0; stT = now; if (rn(2)) dirF = -dirF; }
  }
  bunny(by, D, twitch, stretch);

  // ---- Schmetterlinge ----
  for (int i = 0; i < 2; i++) {
    const float a = (float)(now % (9000 + i * 2500)) / (9000.0f + i * 2500.0f) * 6.2832f;
    const int x = 100 + (int)(sinf(a + i * 2.0f) * 40.0f), y = 70 + (int)(sinf(a * 2.0f + i) * 10.0f);
    const bool open = ((now / 120 + (uint32_t)i) & 1) != 0;
    const uint8_t col = i ? O_BF2 : O_BF1;
    fb->drawFastVLine(x, y, 3, O_EYE);
    if (open) { fb->fillRect(x - 3, y - 1, 3, 3, col); fb->fillRect(x + 1, y - 1, 3, 3, col); }
    else { fb->fillRect(x - 1, y - 2, 1, 3, col); fb->fillRect(x + 1, y - 2, 1, 3, col); }
  }

  // ---- Uhrzeit in Pastell ----
  {
    const int s = 4, x = 49, y = 4;
    const int dg[4] = {c.timeOk ? c.hh / 10 : 10, c.timeOk ? c.hh % 10 : 10, c.timeOk ? c.mm / 10 : 10, c.timeOk ? c.mm % 10 : 10};
    const int ox[4] = {0, 6 * s, 16 * s, 22 * s};
    const uint8_t cols[4] = {O_D1, O_D2, O_D3, O_D4};
    for (int k = 0; k < 4; k++) { glyph(x + ox[k], y, dg[k], s, cellS); digCol = cols[k]; glyph(x + ox[k], y, dg[k], s, cellD); }
    digCol = O_ACC;
    cellS(x + 13 * s, y + 2 * s, s, 0, 0); cellD(x + 13 * s, y + 2 * s, s, 0, 0);
    cellS(x + 13 * s, y + 4 * s, s, 0, 0); cellD(x + 13 * s, y + 4 * s, s, 0, 0);
  }
  if (c.showSec && c.timeOk) fb->drawFastHLine(49, 33, (c.ss + 1) * 108 / 60, O_ACC);

  // ---- Countdown ----
  char b[24];
  const int cx = 104;
  if (c.timeOk) {
    int em, ed;
    easterDate(c.year, em, ed);
    int doyE = dayOfYear(c.year, em, ed);
    const int doy = dayOfYear(c.year, c.mon, c.mday);
    if (doy == doyE || doy == doyE + 1) {
      txtSC(cx, 38, TR(D_HAPPY_EASTER), O_ACC, O_WHITE, 2);
      txtSC(cx, 54, TR(D_EASTER), O_TXT, O_WHITE, 2);
    } else {
      int d;
      if (doy < doyE) d = doyE - doy;
      else { easterDate(c.year + 1, em, ed); d = daysInYear(c.year) - doy + dayOfYear(c.year + 1, em, ed); }
      snprintf(b, sizeof(b), "%d", d);
      const char* unit = d == 1 ? TR(D_DAY) : TR(D_DAYS);
      const int w = txtw(b, 2) + 5 + txtw(unit, 1), x0 = cx - w / 2;
      txtS(x0, 38, b, O_ACC, O_WHITE, 2);
      txtS(x0 + txtw(b, 2) + 5, 45, unit, O_TXT, O_WHITE);
      txtSC(cx, 55, TR(D_TO_EASTER), O_DIM, O_WHITE);
    }
    langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
    txt(4, 111, b, O_GTXT);
  }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 111, c.temp, O_GTXT);
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  if (p >= 0.97f) { memset(buf(), O_SKY2, FB_W * FB_H); return; }
  const int h = (int)(p * (FB_H + 10));
  fb->fillRect(0, 0, FB_W, h, O_SKY2);
  for (int i = 0; i < 16; i++) {
    const int x = 5 + i * 10, y = h - 4 - (i * 7) % 9;
    egg(x, y, (uint8_t)(O_E1 + i % 5), O_WHITE, i % 3);
  }
}

}  // namespace f_egg
