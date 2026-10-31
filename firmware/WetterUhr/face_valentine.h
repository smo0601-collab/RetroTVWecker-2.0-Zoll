#pragma once
/*
  face_valentine.h - Zifferblatt "Valentinstag"
  Rosaroter Himmel voller aufsteigender Herzen. Zwei Turteltauben sitzen eng beieinander auf einem
  Ast und schnaebeln ab und zu (dann steigt ein Herzchen auf). Ab und zu fliegt Amors Pfeil quer durchs
  Bild. Darunter der Countdown bis zum Valentinstag (14.02.), am Tag selbst "ALLES LIEBE".
  Zur vollen Minute oeffnet sich der Liebesbrief unten rechts, ein grosses Herz steigt heraus und
  zerplatzt in viele kleine.
  Uebergang: Ein Herz waechst aus der Mitte, bis es alles fuellt.
*/
#include "faces_fx.h"

namespace f_val {
using namespace fc;

enum : uint8_t { V_BG1 = 1, V_BG2, V_BG3, V_H1, V_H2, V_H3, V_H4, V_BRANCH, V_LEAF, V_B1, V_B1D, V_B2, V_B2D,
                 V_BEAK, V_EYE, V_ENV, V_ENV2, V_SEAL, V_ARROW, V_FEATH, V_DIG, V_SH, V_TXT, V_DIM, V_WHITE };

struct Heart { float x, y, v, ph; uint8_t s, col; };
static Heart    hs[10];
static Sparks<40> burst;
static uint32_t lastT, letterT, arrowT, kissT;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(V_BG1, 120, 30, 70); pal(V_BG2, 170, 50, 100); pal(V_BG3, 214, 90, 136);
  pal(V_H1, 255, 60, 90); pal(V_H2, 255, 130, 170); pal(V_H3, 255, 210, 226); pal(V_H4, 220, 20, 60);
  pal(V_BRANCH, 100, 50, 40); pal(V_LEAF, 90, 170, 90);
  pal(V_B1, 250, 250, 250); pal(V_B1D, 200, 200, 214); pal(V_B2, 255, 180, 200); pal(V_B2D, 220, 130, 160);
  pal(V_BEAK, 255, 170, 40); pal(V_EYE, 30, 10, 20);
  pal(V_ENV, 255, 244, 232); pal(V_ENV2, 220, 200, 190); pal(V_SEAL, 220, 30, 60);
  pal(V_ARROW, 250, 220, 120); pal(V_FEATH, 255, 255, 255);
  pal(V_DIG, 255, 250, 252); pal(V_SH, 110, 10, 50); pal(V_TXT, 255, 236, 244); pal(V_DIM, 255, 196, 220); pal(V_WHITE, 255, 255, 255);
  for (int i = 0; i < 10; i++) hs[i] = {(float)rn(FB_W), (float)rn(FB_H), 6.0f + rf() * 8.0f, rf() * 6.28f, (uint8_t)(1 + (rn(3) == 0)), (uint8_t)(V_H1 + rn(3))};
  burst.clear();
  lastT = now; letterT = 0; arrowT = now + 8000; kissT = now + 4000; lastMm = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, V_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, V_DIG); }

// Herz 7x6 (mal s), (x,y) = links oben
static void heart(int x, int y, int s, uint8_t col) {
  static const uint8_t H[6] = {0x36, 0x7F, 0x7F, 0x3E, 0x1C, 0x08};
  for (int r = 0; r < 6; r++)
    for (int k = 0; k < 7; k++)
      if (H[r] & (0x40 >> k)) fb->fillRect(x + k * s, y + r * s, s, s, col);
}

static void bird(int x, int y, int d, uint8_t body, uint8_t dark, bool kiss) {
  fb->fillCircle(x, y, 5, body);
  fb->fillTriangle(x - 4 * d, y + 1, x - 10 * d, y + 3, x - 4 * d, y + 4, dark);       // Schwanz
  fb->fillCircle(x - 1 * d, y + 1, 3, dark);                                            // Fluegel
  const int hx = x + (kiss ? 5 : 4) * d, hy = y - 5;
  fb->fillCircle(hx, hy, 3, body);
  fb->drawPixel(hx + d, hy - 1, V_EYE);
  fb->drawPixel(hx + 3 * d, hy, V_BEAK); fb->drawPixel(hx + 4 * d, hy, V_BEAK);
  fb->drawPixel(x - 1, y + 5, V_BEAK); fb->drawPixel(x + 1, y + 5, V_BEAK);             // Fuesse
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) letterT = now;
    lastMm = c.mm;
  }

  // ---- Hintergrund ----
  fb->fillRect(0, 0, FB_W, 40, V_BG1);
  fb->fillRect(0, 40, FB_W, 40, V_BG2);
  fb->fillRect(0, 80, FB_W, 40, V_BG3);
  for (int y = 38; y < 42; y++) for (int x = (y & 1); x < FB_W; x += 2) fb->drawPixel(x, y, V_BG2);
  for (int y = 78; y < 82; y++) for (int x = (y & 1); x < FB_W; x += 2) fb->drawPixel(x, y, V_BG3);

  // ---- aufsteigende Herzen ----
  for (int i = 0; i < 10; i++) {
    Heart& h = hs[i];
    h.y -= h.v * dt; h.ph += dt * 1.6f;
    if (h.y < -14) { h.y = FB_H + 2; h.x = (float)rn(FB_W); h.col = (uint8_t)(V_H1 + rn(3)); }
    heart((int)(h.x + sinf(h.ph) * 4.0f), (int)h.y, h.s, h.col);
  }

  // ---- Amors Pfeil ----
  if ((int32_t)(now - arrowT) >= 0) {
    const float a = secs(now, arrowT) / 1.6f;
    if (a > 1.0f) arrowT = now + 15000 + (uint32_t)rn(15000);
    else {
      const int x = (int)(-30.0f + a * 220.0f), y = 70 - (int)(sinf(a * 3.1416f) * 10.0f);
      fb->drawFastHLine(x, y, 22, V_ARROW);
      fb->fillTriangle(x + 22, y - 2, x + 22, y + 2, x + 26, y, V_H4);
      fb->drawLine(x, y, x - 3, y - 3, V_FEATH); fb->drawLine(x + 3, y, x, y - 3, V_FEATH);
      fb->drawLine(x, y, x - 3, y + 3, V_FEATH); fb->drawLine(x + 3, y, x, y + 3, V_FEATH);
      if ((now / 80) & 1) heart(x + 10, y - 8, 1, V_H3);
    }
  }

  // ---- Ast mit Turteltauben ----
  fb->drawLine(0, 104, 64, 94, V_BRANCH); fb->drawLine(0, 105, 64, 95, V_BRANCH); fb->drawLine(0, 106, 50, 98, V_BRANCH);
  fb->drawLine(50, 96, 58, 88, V_BRANCH);
  fb->fillCircle(58, 87, 2, V_LEAF); fb->fillCircle(8, 100, 2, V_LEAF); fb->fillCircle(64, 93, 2, V_LEAF);
  const float kt = secs(now, kissT);
  const bool kiss = (int32_t)(now - kissT) >= 0 && kt < 1.2f;
  if ((int32_t)(now - kissT) >= 0 && kt > 3.0f) kissT = now + 6000 + (uint32_t)rn(6000);
  bird(22, 94, 1, V_B1, V_B1D, kiss);
  bird(40, 91, -1, V_B2, V_B2D, kiss);
  if ((int32_t)(now - kissT) >= 0 && kt < 3.0f) heart(28, 78 - (int)(kt * 8.0f), 1, V_H1);

  // ---- Liebesbrief ----
  const int lx = 124, ly = 92;
  const float lt = letterT ? secs(now, letterT) : 99.0f;
  fb->fillRect(lx, ly, 34, 20, V_ENV);
  fb->drawRect(lx, ly, 34, 20, V_ENV2);
  fb->drawLine(lx, ly + 19, lx + 16, ly + 9, V_ENV2); fb->drawLine(lx + 33, ly + 19, lx + 17, ly + 9, V_ENV2);
  if (lt < 4.0f) {
    fb->fillTriangle(lx, ly, lx + 33, ly, lx + 16, ly - 12, V_ENV2);                        // Klappe offen
    fb->fillTriangle(lx + 2, ly, lx + 31, ly, lx + 16, ly - 10, V_ENV);
    if (lt < 1.4f) heart(lx + 10, ly - 2 - (int)(lt * 22.0f), 2, V_H4);                     // grosses Herz steigt
    else if (lt < 1.5f) {
      for (int k = 0; k < 4; k++) burst.burst((float)(lx + 17), (float)(ly - 30), 7, 38.0f, (uint8_t)(V_H1 + k % 3));
    }
  } else {
    fb->fillTriangle(lx, ly, lx + 33, ly, lx + 16, ly + 11, V_ENV2);                        // Klappe zu
    fb->fillTriangle(lx + 2, ly, lx + 31, ly, lx + 16, ly + 9, V_ENV);
    heart(lx + 13, ly + 7, 1, V_SEAL);
  }
  burst.step(dt, 14.0f);
  for (int k = 0; k < 40; k++) {
    const Spark& p = burst.s[k];
    if (p.life > 0) { fb->drawPixel((int)p.x, (int)p.y, p.col); fb->drawPixel((int)p.x - 1, (int)p.y - 1, p.col); fb->drawPixel((int)p.x + 1, (int)p.y - 1, p.col); }
  }

  // ---- Uhrzeit ----
  drawTime(26, 4, 4, c, cellS);
  drawTime(26, 4, 4, c, cellD);
  if (c.showSec && c.timeOk) {
    const int n = c.ss / 6;                                          // 10 kleine Herzen fuer die Sekunden
    for (int k = 0; k < 10; k++) heart(27 + k * 11, 34, 1, k < n ? V_H1 : V_BG2);
  }

  // ---- Countdown ----
  char b[24];
  const int cx = 80;
  if (c.timeOk) {
    if (c.mon == 2 && c.mday == 14) {
      const uint8_t a = ((now / 600) & 1) ? V_WHITE : V_H3;
      txtSC(cx, 46, TR(D_VAL_L1), a, V_SH, 2);
      txtSC(cx, 63, TR(D_VAL_L2), V_TXT, V_SH);
    } else {
      const int d = daysUntil(c.year, c.mon, c.mday, 2, 14);
      snprintf(b, sizeof(b), "%d", d);
      const char* unit = d == 1 ? TR(D_DAY) : TR(D_DAYS);
      const int us = (txtw(b, 3) + 6 + txtw(unit, 2) > 2 * (cx < FB_W - cx ? cx : FB_W - cx) - 4) ? 1 : 2;   // lange Woerter kleiner
        const int w = txtw(b, 3) + 6 + txtw(unit, us), x0 = cx - w / 2;
      txtS(x0, 44, b, V_WHITE, V_SH, 3);
      txtS(x0 + txtw(b, 3) + 6, 51 + (2 - us) * 4, unit, V_TXT, V_SH, us);
      txtSC(cx, 68, TR(D_TO_VAL), V_DIM, V_SH);
    }
    langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
    txtS(94 - txtw(b) / 2, 101, b, V_TXT, V_SH);
  }
  if (c.hasWx) tempTxt(94 - tempW(c.temp, 1) / 2, 111, c.temp, V_TXT);
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  if (p >= 0.97f) { memset(buf(), V_H4, FB_W * FB_H); return; }
  const int s = 1 + (int)(p * 34.0f);
  heart(80 - (7 * s) / 2, 60 - (6 * s) / 2 - s, s, V_H4);
}

}  // namespace f_val
