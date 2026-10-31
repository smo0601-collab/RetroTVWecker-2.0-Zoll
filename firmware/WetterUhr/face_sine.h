#pragma once
/*
  face_sine.h - Zifferblatt "Sinuskurve"
  Oszilloskop-Optik: dunkles Raster, drei Wellen laufen ueber den Schirm - eine cyanfarbene
  Sinuswelle, eine gegenlaeufige magentafarbene Oberwelle und die gelbe Summe beider.
  Die Uhrzeit steht gross in der Mitte, hinterlegt mit einem dunklen Schatten, damit die
  Kurven die Ziffern nicht stoeren. Ein Punkt wandert als Sekundenzeiger auf der unteren
  Skala und reitet dabei auf der gelben Kurve. Zur vollen Minute schlaegt die Amplitude kurz aus.
  Die Sinuswerte kommen aus einer kleinen Tabelle (kein sinf pro Pixel).
  Uebergang: Wellenfoermige Blende von links nach rechts.
*/
#include "faces_fx.h"

namespace f_sine {
using namespace fc;

enum : uint8_t { S_BG = 1, S_GRID, S_AXIS, S_A, S_A2, S_B, S_B2, S_SUM, S_SUM2, S_DIG, S_SH, S_DIM, S_SEC, S_EDGE };

static int8_t  LUT[256];                           // sin * 127
static uint32_t boostT, lastT;
static uint8_t lastMm;

static void enter(uint32_t now) {
  pal(S_BG, 3, 12, 16); pal(S_GRID, 14, 44, 52); pal(S_AXIS, 24, 80, 90);
  pal(S_A, 40, 230, 255); pal(S_A2, 16, 90, 110);
  pal(S_B, 255, 60, 210); pal(S_B2, 100, 24, 86);
  pal(S_SUM, 255, 232, 60); pal(S_SUM2, 110, 98, 24);
  pal(S_DIG, 235, 255, 250); pal(S_SH, 2, 8, 12); pal(S_DIM, 110, 170, 180);
  pal(S_SEC, 255, 140, 40); pal(S_EDGE, 120, 255, 255);
  for (int i = 0; i < 256; i++) LUT[i] = (int8_t)lroundf(sinf(i * 6.2831853f / 256.0f) * 127.0f);
  boostT = 0; lastMm = 255; lastT = now;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x - 1, y - 1, s + 2, s + 2, S_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, S_DIG); }

static inline int sinI(int idx, int amp) { return (LUT[idx & 255] * amp) >> 7; }

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) boostT = now;
    lastMm = c.mm;
  }
  float boost = 0.0f;                                // 1 -> 0 ueber 1,4 s
  if (boostT) { boost = 1.0f - secs(now, boostT) / 1.4f; if (boost < 0) { boost = 0; boostT = 0; } }

  fb->fillScreen(S_BG);

  // ---- Raster ----
  const int MID = 60;
  for (int gy = MID - 48; gy <= MID + 48; gy += 16)
    for (int gx = 0; gx < FB_W; gx += 16) fb->drawPixel(gx, gy, S_GRID);
  for (int gx = 0; gx < FB_W; gx += 4) fb->drawPixel(gx, MID, S_AXIS);
  for (int gy = MID - 48; gy <= MID + 48; gy += 4) fb->drawPixel(80, gy, S_AXIS);

  // ---- Wellen ----
  const int ampA = 24 + (LUT[(now / 45) & 255] >> 4) + (int)(boost * 14.0f);   // atmet langsam
  const int ampB = 13 + (int)(boost * 8.0f);
  const int phA = (int)(now / 6), phB = (int)(now / 9);
  const bool flashW = boost > 0.55f;
  int pyA = 0, pyB = 0, pyS = 0, sumAtSec = MID;
  const int secX = (c.timeOk) ? 8 + (c.ss * 144) / 59 : -1;
  for (int x = 0; x < FB_W; x++) {
    const int ya = MID - sinI(x * 3 + phA, ampA);
    const int yb = MID - sinI(x * 5 - phB, ampB);
    const int ys = MID - (sinI(x * 3 + phA, ampA) + sinI(x * 5 - phB, ampB)) * 3 / 4;
    if (x > 0) {
      fb->drawLine(x - 1, pyA, x, ya, flashW ? I_WHITE : S_A);
      fb->drawLine(x - 1, pyB, x, yb, flashW ? I_WHITE : S_B);
      fb->drawLine(x - 1, pyS - 1, x, ys - 1, S_SUM2);
      fb->drawLine(x - 1, pyS + 1, x, ys + 1, S_SUM2);
      fb->drawLine(x - 1, pyS, x, ys, S_SUM);
    }
    pyA = ya; pyB = yb; pyS = ys;
    if (x == secX) sumAtSec = ys;
  }

  // ---- Zeit mit Schatten ----
  drawTime(26, 46, 4, c, cellS);
  drawTime(26, 46, 4, c, cellD);

  // ---- Kopf- und Fusszeile ----
  fb->fillRect(0, 0, FB_W, 11, S_BG);
  char b[24];
  if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 2, b, S_DIM);
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 2, c.temp, S_DIM);
  else         txtr(FB_W - 4, 2, "--", S_DIM);
  fb->drawFastHLine(0, 11, FB_W, S_GRID);

  fb->fillRect(0, 107, FB_W, 13, S_BG);
  fb->drawFastHLine(0, 107, FB_W, S_GRID);
  for (int x = 8; x <= 152; x += 12) fb->drawFastVLine(x, 112, 3, S_AXIS);
  fb->drawFastHLine(8, 113, 145, S_GRID);
  if (c.hasWx) {
    snprintf(b, sizeof(b), "%d%%", c.hum);
    txt(4, 110, b, S_DIM);
    snprintf(b, sizeof(b), "%dkm/h", c.wind);
    txtr(FB_W - 4, 110, b, S_DIM);
  }

  // ---- Sekundenpunkt: reitet auf der gelben Kurve ----
  if (c.timeOk && c.showSec) {
    const int sy = icl(sumAtSec, 14, 104);
    if (sy + 3 < 106) fb->drawFastVLine(secX, sy + 3, 106 - sy - 3, S_SEC);
    fb->fillRect(secX - 1, sy - 1, 3, 3, S_SEC);
    fb->fillRect(secX - 1, 111, 3, 4, S_SEC);
  }
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  const int reach = (int)(p * (FB_W + 40)) - 20;
  for (int y = 0; y < FB_H; y++) {
    const int e = icl(reach + (LUT[(y * 6) & 255] >> 3), 0, FB_W);
    if (e > 0) memset(bf + y * FB_W, S_BG, e);
    if (e > 0 && e < FB_W) bf[y * FB_W + e] = S_EDGE;
  }
}

}  // namespace f_sine
