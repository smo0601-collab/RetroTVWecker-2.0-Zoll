#pragma once
/*
  face_testcard.h - Zifferblatt "Testbild"
  Ein Fernseh-Testbild wie aus alten Sendepausen (eigene Gestaltung): Raster mit Diagonalen,
  Farbbalken oben, Kreis in der Mitte, Graustufen-Keil unten. Die Uhrzeit steht gross in
  einem schwarzen Streifen, darunter Datum und Temperatur. Ein heller Balken rollt ueber das
  Bild, es "schneit" leicht, und alle paar Sekunden zucken ein paar Stoerstreifen durch.
  Die Reihenfolge der Farbbalken dreht sich mit jeder Minute weiter; dazu gibt es einen kurzen
  Schneeschauer wie beim Programmwechsel. Der Sekundenzeiger ist ein Pfeil unter dem Graukeil.
  Uebergang: Bildroehre schaltet aus (Bild faellt zu einer Linie zusammen) bzw. an.
*/
#include "faces_fx.h"

namespace f_tv {
using namespace fc;

enum : uint8_t { T_BG = 1, T_GRID, T_GRID2, T_DIAG, T_B0, T_B1, T_B2, T_B3, T_B4, T_B5, T_B6,
                 T_G0, T_G1, T_G2, T_G3, T_G4, T_G5, T_G6, T_G7, T_PANEL, T_DIG, T_SH, T_TXT, T_ROLL, T_RING };

static uint32_t lastT, snowUntil, glitchAt, glitchUntil;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(T_BG, 74, 74, 80); pal(T_GRID, 112, 112, 120); pal(T_GRID2, 170, 170, 180); pal(T_DIAG, 96, 96, 104);
  pal(T_B0, 235, 235, 235); pal(T_B1, 235, 235, 20); pal(T_B2, 20, 235, 235); pal(T_B3, 20, 220, 30);
  pal(T_B4, 235, 30, 235); pal(T_B5, 235, 30, 30); pal(T_B6, 30, 30, 235);
  for (int i = 0; i < 8; i++) pal(T_G0 + i, i * 36, i * 36, i * 36 + (i == 7 ? 3 : 0));
  pal(T_PANEL, 6, 6, 10); pal(T_DIG, 245, 245, 235); pal(T_SH, 0, 0, 0); pal(T_TXT, 235, 235, 225);
  pal(T_ROLL, 92, 92, 100); pal(T_RING, 240, 240, 240);
  lastT = now; snowUntil = 0; lastMm = 255; glitchAt = now + 6000; glitchUntil = 0;
}

static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, T_DIG); }

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) snowUntil = now + 220;
    lastMm = c.mm;
  }
  const int shift = c.timeOk ? c.mm % 7 : 0;

  // ---- Grund, rollender Balken, Raster ----
  fb->fillScreen(T_BG);
  const int ry = (int)((now / 28) % 150) - 15;
  fb->fillRect(0, ry, FB_W, 7, T_ROLL);
  for (int x = 0; x < FB_W; x += 10) fb->drawFastVLine(x, 0, FB_H, x == 80 ? T_GRID2 : T_GRID);
  for (int y = 0; y < FB_H; y += 10) fb->drawFastHLine(0, y, FB_W, y == 60 ? T_GRID2 : T_GRID);
  fb->drawLine(0, 0, FB_W - 1, FB_H - 1, T_DIAG);
  fb->drawLine(FB_W - 1, 0, 0, FB_H - 1, T_DIAG);

  // ---- Kreis und Eckkreise ----
  fb->drawCircle(80, 58, 30, T_RING);
  fb->drawCircle(80, 58, 29, T_RING);
  fb->drawCircle(20, 36, 7, T_RING); fb->drawCircle(140, 36, 7, T_RING);
  fb->drawFastHLine(10, 36, 20, T_RING); fb->drawFastVLine(20, 26, 20, T_RING);
  fb->drawFastHLine(130, 36, 20, T_RING); fb->drawFastVLine(140, 26, 20, T_RING);

  // ---- Farbbalken ----
  fb->fillRect(7, 5, 142, 21, T_SH);
  for (int i = 0; i < 7; i++) fb->fillRect(8 + i * 20, 6, 20, 19, T_B0 + (i + shift) % 7);

  // ---- Graukeil und Sekundenpfeil ----
  fb->fillRect(7, 89, 146, 11, T_SH);
  for (int i = 0; i < 8; i++) {
    const int x0 = 8 + i * 18, x1 = 8 + (i + 1) * 18;
    fb->fillRect(x0, 90, x1 - x0, 9, T_G0 + i);
  }
  if (c.timeOk && c.showSec) {
    const int x = 10 + (c.ss * 136) / 59;
    fb->fillTriangle(x - 3, 106, x + 3, 106, x, 101, I_WHITE);
  }

  // ---- Stoerungen (hinter dem Zeitstreifen) ----
  for (int i = 0; i < 10; i++) fb->drawPixel(rn(FB_W), rn(FB_H), rn(2) ? T_GRID2 : T_G1);
  if ((int32_t)(now - glitchAt) >= 0) { glitchUntil = now + 260; glitchAt = now + 7000 + rn(8000); }
  if ((int32_t)(now - glitchUntil) < 0)
    for (int i = 0; i < 7; i++) {
      const int w = 20 + rn(60), x = rn(FB_W - w), y = rn(FB_H);
      fb->drawFastHLine(x, y, w, rn(2) ? I_WHITE : T_G2);
      if (rn(2)) fb->drawFastHLine(x + 3, y + 1, w - 3, T_G1);
    }

  // ---- Zeitstreifen ----
  fb->fillRect(14, 44, 132, 34, T_PANEL);
  fb->drawRect(14, 44, 132, 34, I_WHITE);
  fb->drawRect(16, 46, 128, 30, T_GRID);
  drawTime(26, 47, 4, c, cellD);

  // ---- Datum links, Wetter rechts, Name in der Mitte ----
  char b[24];
  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  fb->fillRect(0, 107, FB_W, 12, T_SH);
  txt(5, 110, b, T_TXT);
  if (c.hasWx) tempTxt(FB_W - 5 - tempW(c.temp, 1), 110, c.temp, T_TXT);
  else         txtr(FB_W - 5, 110, "--", T_TXT);

  // ---- Schneeschauer beim Minutenwechsel ----
  if ((int32_t)(now - snowUntil) < 0)
    for (int i = 0; i < 2600; i++) fb->drawPixel(rn(FB_W), rn(FB_H), T_G0 + rn(8));
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  const int h = (int)((1.0f - p) * 60.0f);                      // verbleibende halbe Bildhoehe
  for (int y = 0; y < FB_H; y++) {
    if (y >= 60 - h && y < 60 + h) continue;
    memset(bf + y * FB_W, 0, FB_W);
  }
  if (h < 4) {                                                   // Linie schrumpft zum Punkt
    const int len = (int)(FB_W * fcl(1.0f - (p - 0.93f) / 0.07f, 0.02f, 1.0f));
    fb->fillRect(80 - len / 2, 59, len, 2, I_WHITE);
  }
}

}  // namespace f_tv
