#pragma once
/*
  face_dumpling.h - Zifferblatt "Squishy Dumpling"
  Ein knautschiger Teigknoedel (eigene Figur) sitzt in einem Bambus-Daempfkorb, es dampft. Er atmet
  sichtbar (wird etwas breiter und schmaler), blinzelt und macht ab und zu etwas Lustiges:
    - huepft mit Quetschen und Strecken,
    - wird von Essstaebchen angestupst, quetscht sich zusammen ("SQUISH!") und wabbelt nach,
    - wird rot, und aus seinem Zipfel pufft Dampf,
    - schaut nach links und rechts, schuettelt sich.
  Nachts (22 bis 6 Uhr) schlaeft er meistens ("Zz"). Zur vollen Minute macht er einen grossen
  Sprung mit platschender Landung ("BOING!").
  Uebergang: Dampfwolken ziehen von unten herauf.
*/
#include "faces_fx.h"

namespace f_dump {
using namespace fc;

enum : uint8_t { D_BG1 = 1, D_BG2, D_DOT, D_DOUGH, D_DOUGH2, D_DOUGH3, D_HI, D_EYE, D_BLUSH, D_MOUTH,
                 D_BAM, D_BAM2, D_BAM3, D_STEAM, D_STICK, D_STICK2, D_DIG, D_SH, D_TXT, D_BUB, D_BTXT, D_ZZ, D_CRUMB };

enum { M_IDLE, M_HOP, M_POKE, M_BLUSH, M_LOOK, M_SHAKE, M_BIG, M_SLEEP };
static uint32_t lastT, mT, nextAct, bubFrom, bubUntil;
static uint8_t  mode, lastMm;
static float    jig, jigV;                  // Wabbel-Feder (Quetschfaktor und Geschwindigkeit)
static char     bub[16];
static Sparks<24> crumbs;

static void enter(uint32_t now) {
  pal(D_BG1, 255, 228, 206); pal(D_BG2, 255, 214, 186); pal(D_DOT, 255, 240, 226);
  pal(D_DOUGH, 252, 248, 240); pal(D_DOUGH2, 232, 222, 206); pal(D_DOUGH3, 200, 186, 166); pal(D_HI, 255, 255, 255);
  pal(D_EYE, 50, 34, 30); pal(D_BLUSH, 255, 150, 160); pal(D_MOUTH, 200, 70, 80);
  pal(D_BAM, 196, 150, 82); pal(D_BAM2, 160, 116, 56); pal(D_BAM3, 222, 182, 112);
  pal(D_STEAM, 255, 250, 246); pal(D_STICK, 150, 84, 44); pal(D_STICK2, 110, 58, 30);
  pal(D_DIG, 120, 70, 50); pal(D_SH, 255, 246, 236); pal(D_TXT, 140, 90, 70);
  pal(D_BUB, 255, 255, 255); pal(D_BTXT, 120, 60, 60); pal(D_ZZ, 150, 140, 200); pal(D_CRUMB, 236, 220, 196);
  lastT = now; mT = now; mode = M_IDLE; nextAct = now + 4000; bubFrom = bubUntil = 0; lastMm = 255; jig = 0; jigV = 0; bub[0] = 0;
  crumbs.clear();
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, D_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, D_DIG); }

static void say(const char* s, uint32_t from, uint32_t ms) { strlcpy(bub, s, sizeof(bub)); bubFrom = from; bubUntil = from + ms; }

// Knoedelkoerper: oben rund, unten flach. (cx, base) = Mitte unten, rx/ry = Halbachsen
static void body(int cx, int base, float rx, float ry, float lean) {
  const int top = base - (int)(2.0f * ry);
  const float cy = base - ry;
  for (int y = top; y <= base; y++) {
    const float v = ((float)y - cy) / ry;                               // -1 oben .. +1 unten
    float w;
    if (v < 0) w = rx * sqrtf(fmaxf(0.0f, 1.0f - v * v));
    else       w = rx * powf(fmaxf(0.0f, 1.0f - powf(v, 4.0f)), 0.25f);  // flacher Boden
    const int sh = (int)(lean * (1.0f - (v + 1.0f) * 0.5f) * ry * 0.4f); // oben seitlich verschoben
    const int x0 = cx - (int)w + sh, x1 = cx + (int)w + sh;
    if (x1 < x0) continue;
    fb->drawFastHLine(x0, y, x1 - x0 + 1, D_DOUGH);
    const int sw = (int)(w * 0.28f) + 1;                                // Schatten rechts
    fb->drawFastHLine(x1 - sw + 1, y, sw, D_DOUGH2);
    if (v > 0.55f) fb->drawFastHLine(x0, y, x1 - x0 + 1, D_DOUGH2);    // Unterseite
    fb->drawPixel(x0, y, D_DOUGH3); fb->drawPixel(x1, y, D_DOUGH3);
  }
  // Glanzpunkt
  fb->fillRect(cx - (int)(rx * 0.5f) + (int)(lean * ry * 0.3f), (int)(cy - ry * 0.6f), 3, 2, D_HI);
  // Falten oben (Knoedelzipfel)
  const int tx = cx + (int)(lean * ry * 0.4f);
  fb->fillRect(tx - 1, top - 2, 3, 3, D_DOUGH);
  fb->drawPixel(tx, top - 3, D_DOUGH2);
  for (int k = -2; k <= 2; k++) {
    if (!k) continue;
    fb->drawLine(tx, top + 1, tx + k * (int)(rx * 0.22f), top + 3 + abs(k) * 2, D_DOUGH3);
  }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  const float t = secs(now, mT);
  const bool night = c.timeOk && (c.hh >= 22 || c.hh < 6);

  // ---- Zustandswechsel ----
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { mode = M_BIG; mT = now; say("BOING!", now + 700, 1600); }
    lastMm = c.mm;
  }
  if (mode == M_SLEEP && !night) { mode = M_IDLE; mT = now; say("GAEHN!", now, 1500); }
  if (mode == M_IDLE && (int32_t)(now - nextAct) >= 0) {
    mT = now;
    if (night) mode = M_SLEEP;
    else {
      const int r = rn(6);
      mode = r == 0 ? M_HOP : r == 1 ? M_POKE : r == 2 ? M_BLUSH : r == 3 ? M_LOOK : r == 4 ? M_SHAKE : M_HOP;
      if (mode == M_BLUSH) say("HIHI!", now + 300, 1500);
    }
    nextAct = now + 7000 + (uint32_t)rn(7000);
  }
  const float tt = secs(now, mT);

  // ---- Feder (Wabbeln) ----
  jigV += (-160.0f * jig - 7.0f * jigV) * dt;
  jig += jigV * dt;

  // ---- Bewegung je Zustand ----
  float hop = 0.0f, sq = 0.0f, lean = 0.0f, look = 0.0f;
  bool closed = false, squint = false, blush = false, steam = false;
  switch (mode) {
    case M_HOP: {
      const int n = (int)(tt / 0.55f);
      const float a = (tt - n * 0.55f) / 0.55f;
      if (n >= 3) { mode = M_IDLE; jigV += 3.0f; break; }
      if (a < 0.2f) sq = 0.25f * sinf(a / 0.2f * 3.1416f);                // ausholen
      else { const float b = (a - 0.2f) / 0.8f; hop = sinf(b * 3.1416f) * 14.0f; sq = -0.12f * sinf(b * 3.1416f); }
      if (a > 0.95f) jigV += 1.5f;
      break;
    }
    case M_BIG: {
      if (tt < 0.25f) sq = 0.3f * sinf(tt / 0.25f * 3.1416f);
      else if (tt < 1.2f) { const float b = (tt - 0.25f) / 0.95f; hop = sinf(b * 3.1416f) * 30.0f; sq = -0.2f * sinf(b * 3.1416f); }
      else {
        if (tt - dt < 1.2f) { jig = 0.45f; jigV = 0; for (int k = 0; k < 3; k++) crumbs.burst(80.0f, 104.0f, 4, 30.0f, D_CRUMB); }
        if (tt > 3.0f) { mode = M_IDLE; }
      }
      squint = tt < 1.2f && tt > 0.25f;
      break;
    }
    case M_POKE: {
      if (tt > 0.6f && tt - dt <= 0.6f) { jig = -0.35f; jigV = 0; say("SQUISH!", now, 1400); }
      squint = tt > 0.6f && tt < 1.6f;
      lean = tt > 0.6f && tt < 1.2f ? -0.6f * (1.2f - tt) / 0.6f : 0.0f;
      if (tt > 2.6f) mode = M_IDLE;
      break;
    }
    case M_BLUSH: blush = true; steam = tt > 0.4f && tt < 2.0f; squint = true; if (tt > 2.6f) mode = M_IDLE; break;
    case M_LOOK:  look = tt < 1.0f ? -1.0f : (tt < 2.0f ? 1.0f : 0.0f); if (tt > 2.6f) mode = M_IDLE; break;
    case M_SHAKE: lean = sinf(tt * 18.0f) * 0.5f * fmaxf(0.0f, 1.0f - tt / 1.5f); if (tt > 1.6f) mode = M_IDLE; break;
    case M_SLEEP: closed = true; if (!night) mode = M_IDLE; break;
    default: break;
  }
  (void)t;
  const float breathe = sinf((float)(now % 3200) / 3200.0f * 6.2832f) * (closed ? 0.05f : 0.03f);
  const float s = sq + jig + breathe;                                      // >0 breit und flach, <0 schmal und hoch
  const float R = 28.0f;
  const float rx = R * (1.0f + s), ry = R * 0.8f * (1.0f - s);

  // ---- Hintergrund ----
  fb->fillRect(0, 0, FB_W, 60, D_BG1);
  fb->fillRect(0, 60, FB_W, 60, D_BG2);
  for (int y = 6; y < FB_H; y += 12)
    for (int x = ((y / 12) & 1) * 6 + 3; x < FB_W; x += 12) fb->fillRect(x, y, 2, 2, D_DOT);

  // ---- Datum und Temperatur ----
  char b[24];
  if (c.timeOk) { snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon); txt(4, 38, b, D_TXT); }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 38, c.temp, D_TXT);

  // ---- Korb hinten ----
  const int cx = 80, base = 106;
  fb->fillRoundRect(cx - 46, 98, 92, 8, 4, D_BAM2);
  fb->drawFastHLine(cx - 44, 99, 88, D_BAM3);

  // ---- Knoedel ----
  const int by = base - (int)hop;
  body(cx, by, rx, ry, lean);
  // Gesicht
  const float cy = by - ry;
  const int ex = (int)(rx * 0.36f), ey = (int)(cy + ry * 0.08f), lx = (int)(look * 3.0f) + (int)(lean * ry * 0.25f);
  const bool blink = ((now / 180) % 22) == 0;
  for (int k = -1; k <= 1; k += 2) {
    const int x = cx + k * ex + lx;
    if (closed || blink) fb->drawFastHLine(x - 2, ey + 1, 4, D_EYE);
    else if (squint) {                                                   // > <
      fb->drawLine(x + 2 * k, ey - 1, x - k, ey + 1, D_EYE);
      fb->drawLine(x - k, ey + 1, x + 2 * k, ey + 3, D_EYE);
    } else {
      fb->fillRect(x - 1, ey - 1, 3, 4, D_EYE);
      fb->drawPixel(x, ey - 1, D_HI);
    }
    // Wangen
    fb->fillRect(x + k * 3 - 2, ey + 4, 4, 2, blush ? D_MOUTH : D_BLUSH);
  }
  // Mund (kleines w), beim Schlafen ein o
  const int mx = cx + lx, my = ey + 5;
  if (closed) fb->drawCircle(mx, my + 1, 1, D_MOUTH);
  else if (mode == M_BIG && tt < 1.2f) fb->fillRoundRect(mx - 2, my, 5, 4, 2, D_MOUTH);
  else { fb->drawPixel(mx - 2, my, D_MOUTH); fb->drawPixel(mx - 1, my + 1, D_MOUTH); fb->drawPixel(mx, my, D_MOUTH);
         fb->drawPixel(mx + 1, my + 1, D_MOUTH); fb->drawPixel(mx + 2, my, D_MOUTH); }
  // Dampf aus dem Zipfel
  if (steam) {
    const int top = by - (int)(2.0f * ry) - 4;
    for (int k = 0; k < 3; k++) {
      const float a = fmodf(tt * 1.5f + k * 0.33f, 1.0f);
      fb->fillCircle(cx + (int)(sinf(a * 9.0f + k) * 3.0f), top - (int)(a * 16.0f), 2 - (int)(a * 2.0f), D_STEAM);
    }
  }

  // ---- Korb vorne (verdeckt den Knoedelboden) ----
  fb->fillRect(cx - 48, 104, 96, 16, D_BAM);
  fb->drawFastHLine(cx - 48, 104, 96, D_BAM3);
  fb->drawFastHLine(cx - 48, 105, 96, D_BAM3);
  for (int y = 109; y < FB_H; y += 4) fb->drawFastHLine(cx - 48, y, 96, D_BAM2);
  for (int x = cx - 44; x < cx + 48; x += 8) fb->drawFastVLine(x + ((x / 8) & 1) * 2, 106, 14, D_BAM2);

  // Dampf aus dem Korb
  for (int k = 0; k < 4; k++) {
    const float a = fmodf((float)now / 2600.0f + k * 0.25f, 1.0f);
    const int x = (k < 2 ? 38 + k * 8 : 112 + (k - 2) * 8) + (int)(sinf(a * 8.0f + k) * 3.0f), y = 100 - (int)(a * 34.0f);
    if (a < 0.85f) { fb->drawPixel(x, y, D_STEAM); fb->drawPixel(x + 1, y - 1, D_STEAM); fb->drawPixel(x, y - 2, D_STEAM); }
  }

  // ---- Essstaebchen beim Anstupsen ----
  if (mode == M_POKE) {
    const float a = tt < 0.6f ? tt / 0.6f : (tt < 1.0f ? 1.0f : fmaxf(0.0f, 1.0f - (tt - 1.0f) / 0.6f));
    const int tipX = FB_W + 10 - (int)(a * (FB_W + 10 - (cx + (int)rx + 2)));
    for (int k = 0; k < 2; k++) {
      const int y = (int)cy - 2 + k * 5;
      fb->drawLine(tipX, y, tipX + 70, y - 14 + k * 3, D_STICK);
      fb->drawLine(tipX, y + 1, tipX + 70, y - 13 + k * 3, D_STICK2);
    }
  }

  // Kruemel bei der Landung
  crumbs.step(dt, 60.0f);
  crumbs.draw();

  // ---- Zz beim Schlafen ----
  if (closed) {
    const float a = fmodf((float)now / 2200.0f, 1.0f);
    const int zx = cx + (int)rx - 2 + (int)(a * 10.0f), zy = (int)(cy - ry) - (int)(a * 14.0f);
    txt(zx, zy, "Z", D_ZZ);
    if (a > 0.4f) txt(zx + 8, zy - 9, "z", D_ZZ);
  }

  // ---- Uhrzeit ----
  drawTime(26, 4, 4, c, cellS);
  drawTime(26, 4, 4, c, cellD);
  if (c.showSec && c.timeOk) fb->drawFastHLine(26, 33, (c.ss + 1) * 108 / 60, D_BLUSH);

  // ---- Sprechblase ----
  if (bub[0] && (int32_t)(now - bubFrom) >= 0 && (int32_t)(bubUntil - now) > 0) {
    const int w = txtw(bub) + 8, bx = icl(cx + (int)rx - 4, 2, FB_W - w - 2), byy = icl((int)(cy - ry) - 16, 40, 90);
    fb->fillRoundRect(bx, byy, w, 12, 3, D_BUB);
    fb->fillTriangle(bx + 2, byy + 10, bx + 7, byy + 10, bx - 2, byy + 15, D_BUB);
    txt(bx + 4, byy + 3, bub, D_BTXT);
  }
}

static void cover(float p, bool, uint32_t now) {
  if (p <= 0.0f) return;
  if (p >= 0.97f) { memset(buf(), D_STEAM, FB_W * FB_H); return; }
  const int top = FB_H - (int)(p * (FB_H + 30));
  for (int k = 0; k < 12; k++) {
    const int x = k * 15 + (int)(sinf((float)now * 0.004f + k) * 4.0f);
    fb->fillCircle(x, top + (k % 3) * 6, 14, D_STEAM);
  }
  if (top + 8 < FB_H) fb->fillRect(0, top + 8, FB_W, FB_H - top - 8, D_STEAM);
}

}  // namespace f_dump
