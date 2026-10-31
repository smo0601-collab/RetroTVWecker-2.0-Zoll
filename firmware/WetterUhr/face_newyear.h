#pragma once
/*
  face_newyear.h - Zifferblatt "Silvester"
  Nachthimmel ueber einer Stadtsilhouette, Feuerwerksraketen steigen auf und zerplatzen als Ring,
  Chrysantheme, goldene Trauerweide oder Knistersterne. Oben die Uhrzeit, darunter der Countdown bis
  zum neuen Jahr (Tage, am 31.12. Stunden:Minuten:Sekunden). In der letzten Minute laufen die Sekunden
  riesengross herunter, um Mitternacht kommt "PROSIT NEUJAHR" mit Dauerfeuerwerk, am 1. Januar
  "FROHES NEUES JAHR". Je naeher Mitternacht, desto mehr Raketen.
  Die Uhr schaltet an Silvester von 23:30 bis 00:30 automatisch auf dieses Zifferblatt (abschaltbar).
  Uebergang: Ein Lichtblitz.
*/
#include "faces_fx.h"

namespace f_nye {
using namespace fc;

enum : uint8_t { N_SKY1 = 1, N_SKY2, N_SKY3, N_ST, N_CITY, N_CITY2, N_WIN, N_WIN2,
                 N_R1, N_R1D, N_R2, N_R2D, N_R3, N_R3D, N_R4, N_R4D, N_R5, N_R5D, N_GOLD, N_GOLDD, N_WHITE, N_TRAIL,
                 N_DIG, N_SH, N_TXT, N_DIM, N_FLASH };

struct Part { float x, y, vx, vy, age, life; uint8_t col, dim, kind; };   // kind 0 normal, 1 Weide, 2 Knister
struct Rocket { float x, y, vy, top; uint8_t col, kind; bool on; };
#define NPART 280
static Part     ps[NPART];
static Rocket   rk[6];
static uint32_t lastT, nextLaunch, flashT;
static uint8_t  lastMm, lastSs;

static void enter(uint32_t now) {
  pal(N_SKY1, 4, 6, 24); pal(N_SKY2, 10, 14, 44); pal(N_SKY3, 22, 26, 70); pal(N_ST, 200, 210, 255);
  pal(N_CITY, 8, 8, 16); pal(N_CITY2, 18, 18, 32); pal(N_WIN, 255, 210, 110); pal(N_WIN2, 150, 110, 60);
  pal(N_R1, 255, 80, 100); pal(N_R1D, 210, 50, 80); pal(N_R2, 90, 230, 255); pal(N_R2D, 50, 150, 210);
  pal(N_R3, 150, 255, 130); pal(N_R3D, 80, 190, 80); pal(N_R4, 230, 130, 255); pal(N_R4D, 160, 80, 210);
  pal(N_R5, 255, 170, 70); pal(N_R5D, 210, 110, 40); pal(N_GOLD, 255, 224, 130); pal(N_GOLDD, 200, 150, 60);
  pal(N_WHITE, 255, 255, 255); pal(N_TRAIL, 255, 200, 140);
  pal(N_DIG, 255, 246, 220); pal(N_SH, 20, 10, 50); pal(N_TXT, 230, 236, 255); pal(N_DIM, 150, 160, 210); pal(N_FLASH, 200, 210, 255);
  for (int i = 0; i < NPART; i++) ps[i].life = 0;
  for (int i = 0; i < 6; i++) rk[i].on = false;
  lastT = now; nextLaunch = now + 800; flashT = 0; lastMm = 255; lastSs = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, N_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, N_DIG); }
static uint8_t bigCol = N_GOLD;
static void cellB(int x, int y, int s, int, int) { fb->fillRect(x, y, s - 1, s - 1, bigCol); }
static void cellBS(int x, int y, int s, int, int) { fb->fillRect(x + 2, y + 2, s - 1, s - 1, N_SH); }

static void launch() {
  for (int i = 0; i < 6; i++)
    if (!rk[i].on) {
      static const uint8_t cols[5] = {N_R1, N_R2, N_R3, N_R4, N_R5};
      rk[i] = {10.0f + (float)rn(140), 92.0f, -(66.0f + rf() * 22.0f), 10.0f + (float)rn(56), cols[rn(5)], (uint8_t)rn(4), true};
      return;
    }
}

static void spawn(float x, float y, float vx, float vy, float life, uint8_t col, uint8_t dim, uint8_t kind) {
  for (int k = 0; k < NPART; k++)
    if (ps[k].life <= 0) { ps[k] = {x, y, vx, vy, 0.0f, life, col, dim, kind}; return; }
}

static void explode(const Rocket& r) {
  const uint8_t dim = (uint8_t)(r.col + 1);
  switch (r.kind) {
    case 0:                                                       // Ring
      for (int i = 0; i < 32; i++) { const float a = i * 0.19635f; spawn(r.x, r.y, cosf(a) * 40.0f, sinf(a) * 40.0f, 1.2f, r.col, dim, 0); }
      break;
    case 1:                                                       // Chrysantheme
      for (int i = 0; i < 30; i++) { const float a = rf() * 6.2832f, v = 14.0f + rf() * 32.0f; spawn(r.x, r.y, cosf(a) * v, sinf(a) * v, 1.0f + rf() * 0.6f, r.col, dim, 0); }
      break;
    case 2:                                                       // goldene Trauerweide
      for (int i = 0; i < 26; i++) { const float a = rf() * 6.2832f, v = 8.0f + rf() * 20.0f; spawn(r.x, r.y, cosf(a) * v, sinf(a) * v - 6.0f, 2.2f + rf() * 0.8f, N_GOLD, N_GOLDD, 1); }
      break;
    default:                                                      // Doppelring mit Knistern
      for (int i = 0; i < 16; i++) { const float a = i * 0.3927f; spawn(r.x, r.y, cosf(a) * 44.0f, sinf(a) * 44.0f, 1.1f, r.col, dim, 2);
                                     spawn(r.x, r.y, cosf(a) * 22.0f, sinf(a) * 22.0f, 1.0f, N_WHITE, r.col, 2); }
  }
}

static void fireworks(uint32_t now, float dt) {
  for (int i = 0; i < 6; i++) {
    Rocket& r = rk[i];
    if (!r.on) continue;
    r.y += r.vy * dt; r.vy += 22.0f * dt;
    r.x += sinf(r.y * 0.3f) * 0.15f;
    if (r.y <= r.top || r.vy > -8.0f) { explode(r); r.on = false; continue; }
    fb->drawPixel((int)r.x, (int)r.y, N_WHITE);
    fb->drawPixel((int)r.x, (int)r.y + 1, N_TRAIL);
    fb->drawPixel((int)r.x, (int)r.y + 3, N_GOLDD);
    if ((now / 50) & 1) fb->drawPixel((int)r.x, (int)r.y + 5, N_GOLDD);
  }
  for (int k = 0; k < NPART; k++) {
    Part& p = ps[k];
    if (p.life <= 0) continue;
    p.age += dt;
    if (p.age >= p.life) { p.life = 0; continue; }
    const float drag = p.kind == 1 ? 1.6f : 1.1f;
    p.vx *= (1.0f - drag * dt); p.vy *= (1.0f - drag * dt);
    p.vy += (p.kind == 1 ? 9.0f : 14.0f) * dt;
    p.x += p.vx * dt; p.y += p.vy * dt;
    const float f = p.age / p.life;
    uint8_t col = f < 0.7f ? p.col : p.dim;
    if (p.kind == 2 && f > 0.6f) col = ((now / 40 + (uint32_t)k) & 1) ? N_WHITE : N_SKY1;   // Knistern
    const int x1 = (int)p.x, y1 = (int)p.y;
    if (p.kind == 1) {                                              // Weide: haengender Goldschweif
      if (f < 0.75f) fb->drawLine(x1, y1, x1 - (int)(p.vx * 0.12f), y1 - 3, N_GOLDD);
      fb->drawPixel(x1, y1, col);
    } else {                                                        // Leuchtspur entlang der Flugrichtung
      if (f < 0.6f) fb->drawLine(x1, y1, (int)(p.x - p.vx * 0.13f), (int)(p.y - p.vy * 0.13f), p.dim);
      fb->drawPixel(x1, y1, col);
      if (f < 0.3f) fb->drawPixel(x1, y1, N_WHITE);
    }
  }

}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);

  // ---- Zeitbezug: Sekunden bis Neujahr ----
  long toNY = 999999;
  bool party = false, jan1 = false, nye = false;
  int nyYear = c.year + 1;
  if (c.timeOk && c.unixT > 0) {
    toNY = secsUntilLocal(c.unixT, c.year + 1, 1, 1, 0);
    nye = (c.mon == 12 && c.mday == 31);
    jan1 = (c.mon == 1 && c.mday == 1);
    party = jan1 && c.hh == 0;                                    // erste Stunde des neuen Jahres
    if (jan1) nyYear = c.year;
  }
  const bool final60 = nye && toNY <= 60 && toNY > 0;

  // ---- Raketen-Takt ----
  uint32_t gap = 2500 + (uint32_t)rn(2500);
  if (party && c.mm < 15) gap = 220 + (uint32_t)rn(200);
  else if (party)          gap = 600 + (uint32_t)rn(600);
  else if (final60 || (nye && toNY < 600)) gap = 700 + (uint32_t)rn(500);
  else if (nye && c.hh >= 18) gap = 1800 + (uint32_t)rn(1500);
  else if (jan1)           gap = 2500 + (uint32_t)rn(2500);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { launch(); launch(); launch(); }
    if (jan1 && c.hh == 0 && c.mm == 0 && c.ss == 0 && lastSs != 0) { flashT = now; for (int i = 0; i < 6; i++) launch(); }
    lastMm = c.mm; lastSs = c.ss;
  }
  if ((int32_t)(now - nextLaunch) >= 0) { launch(); nextLaunch = now + gap; }

  // ---- Himmel ----
  fb->fillRect(0, 0, FB_W, 40, N_SKY1);
  fb->fillRect(0, 40, FB_W, 30, N_SKY2);
  fb->fillRect(0, 70, FB_W, 30, N_SKY3);
  for (int i = 0; i < 24; i++) {
    const uint32_t h = (uint32_t)i * 2654435761u + 4321u;
    const int x = (int)((h >> 8) % 158) + 1, y = (int)((h >> 20) % 76) + 1;
    if (((now / (600 + (h & 255))) + (uint32_t)i) % 4) fb->drawPixel(x, y, N_ST);
  }

  // ---- Stadt ----
  static const uint8_t BH[20] = {22, 30, 18, 36, 26, 14, 40, 28, 20, 34, 24, 16, 30, 44, 22, 26, 18, 32, 24, 20};
  for (int i = 0; i < 20; i++) {
    const int x = i * 8, h = BH[i], y = FB_H - h;
    fb->fillRect(x, y, 8, h, (i & 1) ? N_CITY : N_CITY2);
    for (int wy = y + 3; wy < FB_H - 3; wy += 5)
      for (int wx = x + 2; wx < x + 7; wx += 3) {
        const uint32_t hh = (uint32_t)(wx * 31 + wy * 17);
        if ((hh % 5) < 2) fb->drawPixel(wx, wy, ((hh + now / 4000) % 7) ? N_WIN : N_WIN2);
      }
  }
  fb->drawFastVLine(108, FB_H - 56, 12, N_CITY2);                 // Turmspitze
  fb->fillTriangle(104, FB_H - 44, 112, FB_H - 44, 108, FB_H - 52, N_CITY2);

  // ---- Lichtblitz um Mitternacht ----
  const float ft = flashT ? secs(now, flashT) : 99.0f;
  if (ft < 0.25f) coverDither(1.0f - ft * 4.0f, N_FLASH);

  char b[32];
  const int cx = 80;
  if (final60) {
    // ---- letzte Minute: riesige Sekunden ----
    const int sLeft = (int)toNY;
    bigCol = (sLeft <= 10 && ((now / 250) & 1)) ? N_R1 : N_GOLD;
    const int s = 8;
    if (sLeft >= 10) {
      glyph(cx - 6 * s + 1, 18, sLeft / 10, s, cellBS); glyph(cx + 1 * s - s / 2 + 1, 18, sLeft % 10, s, cellBS);
      glyph(cx - 6 * s, 18, sLeft / 10, s, cellB); glyph(cx + 1 * s - s / 2, 18, sLeft % 10, s, cellB);
    } else {
      glyph(cx - 5 * s / 2 + 1, 18, sLeft, s, cellBS);
      glyph(cx - 5 * s / 2, 18, sLeft, s, cellB);
    }
    snprintf(b, sizeof(b), TR(D_UNTIL_YEAR), nyYear);
    txtSC(cx, 6, b, N_TXT, N_SH);
  } else {
    // ---- Uhrzeit ----
    drawTime(26, 4, 4, c, cellS);
    drawTime(26, 4, 4, c, cellD);
    if (c.showSec && c.timeOk) fb->drawFastHLine(26, 33, (c.ss + 1) * 108 / 60, N_DIM);

    if (!c.timeOk) {
      txtSC(cx, 46, TR(D_WAIT_TIME), N_DIM, N_SH);
    } else if (party) {
      const uint8_t a = (uint8_t)(N_R1 + 2 * ((now / 300) % 5));
      txtSC(cx, 38, TR(D_CHEERS), a, N_SH, 3);
      snprintf(b, sizeof(b), TR(D_NEWYEAR_Y), nyYear);
      txtSC(cx, 62, b, N_GOLD, N_SH, 2);
    } else if (jan1) {
      txtSC(cx, 40, TR(D_NY_L1), N_GOLD, N_SH, 2);
      snprintf(b, sizeof(b), TR(D_NY_L2), nyYear);
      txtSC(cx, 58, b, N_TXT, N_SH, 2);
    } else if (nye) {
      txtSC(cx, 38, TR(D_NYE), N_GOLD, N_SH, 2);
      snprintf(b, sizeof(b), "%02ld:%02ld:%02ld", toNY / 3600, (toNY % 3600) / 60, toNY % 60);
      txtSC(cx, 55, b, N_DIG, N_SH, 2);
      snprintf(b, sizeof(b), TR(D_UNTIL_YEAR), nyYear);
      txtSC(cx, 71, b, N_DIM, N_SH);
    } else {
      const long d = toNY / 86400;
      snprintf(b, sizeof(b), "%ld", d);
      const char* unit = d == 1 ? TR(D_DAY) : TR(D_DAYS);
      const int us = (txtw(b, 3) + 6 + txtw(unit, 2) > 2 * (cx < FB_W - cx ? cx : FB_W - cx) - 4) ? 1 : 2;   // lange Woerter kleiner
        const int w = txtw(b, 3) + 6 + txtw(unit, us), x0 = cx - w / 2;
      txtS(x0, 40, b, N_DIG, N_SH, 3);
      txtS(x0 + txtw(b, 3) + 6, 47 + (2 - us) * 4, unit, N_TXT, N_SH, us);
      snprintf(b, sizeof(b), TR(D_UNTIL_YEAR), nyYear);
      txtSC(cx, 65, b, N_DIM, N_SH);
    }
  }

  // ---- Fusszeile ----
  if (c.timeOk) {
    langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
    txtS(3, 111, b, N_TXT, N_SH);
  }
  if (c.hasWx) { tempTxt(FB_W - 4 - tempW(c.temp, 1) + 1, 112, c.temp, N_SH); tempTxt(FB_W - 4 - tempW(c.temp, 1), 111, c.temp, N_TXT); }

  fireworks(now, dt);                                              // Feuerwerk liegt vor allem anderen
}

static void cover(float p, bool in, uint32_t) {
  if (p <= 0.0f) return;
  coverDither(p, in ? N_SKY1 : N_FLASH);
}

}  // namespace f_nye
