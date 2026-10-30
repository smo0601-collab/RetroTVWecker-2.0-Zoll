#pragma once
/*
  face_halloween.h - Zifferblatt "Halloween"
  Gruselnacht unter dem Vollmond: Fledermaeuse flattern vorbei, eine Spinne seilt sich ab, ein
  kleines Gespenst schwebt ueber dem Friedhof, im Kuerbis flackert eine Kerze. Rechts der Countdown
  bis Halloween (31.10.), am Tag selbst "HAPPY HALLOWEEN". Zur vollen Minute saust das Gespenst
  heran und ruft "BUH!", der Kuerbis leuchtet hell auf.
  Uebergang: Nebel zieht von unten herauf.
*/
#include "faces_fx.h"

namespace f_hal {
using namespace fc;

enum : uint8_t { H_SKY1 = 1, H_SKY2, H_SKY3, H_ST, H_MOON, H_MOON2, H_HILL, H_HILL2, H_TREE, H_STONE, H_STONE2,
                 H_PUMP, H_PUMP2, H_PUMP3, H_STEM, H_GLOW, H_GLOW2, H_GHOST, H_GHOST2, H_EYE, H_BAT, H_WEB,
                 H_SPID, H_DIG, H_SH, H_TXT, H_ORG, H_DIM, H_FOG, H_GRN };

struct Bat { float x, y, v, ph, amp; };
static Bat      bats[4];
static uint32_t lastT, booT, glowUntil;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(H_SKY1, 16, 6, 34); pal(H_SKY2, 46, 18, 78); pal(H_SKY3, 76, 30, 104); pal(H_ST, 200, 190, 230);
  pal(H_MOON, 250, 240, 196); pal(H_MOON2, 214, 200, 150);
  pal(H_HILL, 10, 4, 18); pal(H_HILL2, 24, 10, 34); pal(H_TREE, 6, 2, 12);
  pal(H_STONE, 120, 116, 136); pal(H_STONE2, 80, 76, 96);
  pal(H_PUMP, 255, 130, 20); pal(H_PUMP2, 214, 92, 10); pal(H_PUMP3, 160, 60, 6); pal(H_STEM, 70, 100, 30);
  pal(H_GLOW, 255, 236, 110); pal(H_GLOW2, 255, 180, 50);
  pal(H_GHOST, 240, 240, 250); pal(H_GHOST2, 180, 180, 210); pal(H_EYE, 10, 10, 20); pal(H_BAT, 4, 2, 8);
  pal(H_WEB, 150, 140, 170); pal(H_SPID, 20, 16, 24);
  pal(H_DIG, 255, 150, 30); pal(H_SH, 40, 0, 20); pal(H_TXT, 236, 220, 255); pal(H_ORG, 255, 180, 60);
  pal(H_DIM, 160, 130, 190); pal(H_FOG, 70, 50, 96); pal(H_GRN, 120, 255, 110);
  for (int i = 0; i < 4; i++) bats[i] = {(float)rn(FB_W), 40.0f + (float)rn(40), 14.0f + rf() * 12.0f, rf() * 6.28f, 3.0f + rf() * 4.0f};
  lastT = now; booT = 0; glowUntil = 0; lastMm = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, H_SH); }
static void cellD(int x, int y, int s, int cx, int cy) { fb->fillRect(x, y, s, s, ((cx + cy) & 3) == 0 ? H_ORG : H_DIG); }

static void bat(int x, int y, bool up) {
  fb->fillRect(x - 1, y, 3, 2, H_BAT);
  fb->drawPixel(x - 1, y - 1, H_BAT); fb->drawPixel(x + 1, y - 1, H_BAT);   // Ohren
  fb->drawPixel(x - 1, y, H_PUMP); fb->drawPixel(x + 1, y, H_PUMP);         // Augen
  if (up) {
    fb->drawLine(x - 2, y, x - 5, y - 3, H_BAT); fb->drawLine(x + 2, y, x + 5, y - 3, H_BAT);
    fb->drawPixel(x - 6, y - 2, H_BAT); fb->drawPixel(x + 6, y - 2, H_BAT);
  } else {
    fb->drawLine(x - 2, y + 1, x - 5, y + 2, H_BAT); fb->drawLine(x + 2, y + 1, x + 5, y + 2, H_BAT);
    fb->drawPixel(x - 6, y + 1, H_BAT); fb->drawPixel(x + 6, y + 1, H_BAT);
  }
}

static void ghost(int x, int y, int s, uint32_t now) {        // s = 1 normal, 2 gross
  const int w = 12 * s, h = 14 * s;
  fb->fillCircle(x + w / 2, y + w / 2, w / 2, H_GHOST);
  fb->fillRect(x, y + w / 2, w, h - w / 2 - 2 * s, H_GHOST);
  for (int k = 0; k < 4; k++) {                                   // gewellter Saum
    const int ox = x + k * 3 * s, off = ((k + (int)(now / 150)) & 1) * s;
    fb->fillRect(ox, y + h - 2 * s, 2 * s, 2 * s + off, H_GHOST);
  }
  fb->drawFastVLine(x + w - 1, y + w / 2, h - w / 2 - 2 * s, H_GHOST2);
  fb->fillRect(x + 3 * s, y + 4 * s, 2 * s, 3 * s, H_EYE);
  fb->fillRect(x + 7 * s, y + 4 * s, 2 * s, 3 * s, H_EYE);
  fb->fillRect(x + 5 * s, y + 8 * s, 2 * s, 2 * s, H_EYE);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { booT = now; glowUntil = now + 2500; }
    lastMm = c.mm;
  }

  // ---- Himmel, Sterne, Mond ----
  fb->fillRect(0, 0, FB_W, 36, H_SKY1);
  fb->fillRect(0, 36, FB_W, 32, H_SKY2);
  fb->fillRect(0, 68, FB_W, 30, H_SKY3);
  for (int i = 0; i < 22; i++) {
    const uint32_t h = (uint32_t)i * 2654435761u + 1234u;
    const int x = (int)((h >> 8) % 158) + 1, y = (int)((h >> 20) % 60) + 1;
    if (((now / (500 + (h & 255))) + (uint32_t)i) % 4) fb->drawPixel(x, y, H_ST);
  }
  fb->fillCircle(22, 21, 15, H_MOON);
  fb->fillCircle(16, 17, 3, H_MOON2); fb->fillCircle(27, 26, 4, H_MOON2); fb->fillCircle(26, 13, 2, H_MOON2);

  // ---- Huegel, Baum, Grabsteine ----
  fb->fillCircle(30, 150, 62, H_HILL2);
  fb->fillCircle(130, 160, 72, H_HILL2);
  fb->fillRect(0, 100, FB_W, 20, H_HILL);
  fb->fillCircle(80, 140, 50, H_HILL);
  // toter Baum
  fb->fillTriangle(10, 100, 24, 100, 17, 46, H_TREE);
  fb->drawLine(17, 60, 4, 48, H_TREE); fb->drawLine(17, 61, 4, 49, H_TREE);
  fb->drawLine(18, 56, 34, 42, H_TREE); fb->drawLine(18, 57, 34, 43, H_TREE);
  fb->drawLine(28, 48, 30, 38, H_TREE); fb->drawLine(8, 52, 6, 42, H_TREE);
  fb->drawLine(18, 72, 30, 66, H_TREE); fb->drawLine(18, 73, 30, 67, H_TREE);
  // Grabsteine
  const int gx[3] = {56, 76, 96}, gh[3] = {14, 18, 12};
  for (int i = 0; i < 3; i++) {
    const int y = 100 - gh[i];
    fb->fillRoundRect(gx[i], y, 12, gh[i] + 2, 5, H_STONE);
    fb->drawFastVLine(gx[i] + 11, y + 3, gh[i] - 2, H_STONE2);
    if (i == 1) { fb->drawFastVLine(gx[i] + 5, y + 4, 7, H_STONE2); fb->drawFastHLine(gx[i] + 3, y + 6, 5, H_STONE2); }
    else { fb->drawFastHLine(gx[i] + 3, y + 5, 6, H_STONE2); fb->drawFastHLine(gx[i] + 3, y + 8, 5, H_STONE2); }
  }

  // ---- Kuerbis ----
  const bool bright = (int32_t)(glowUntil - now) > 0;
  const int px = 134, py = 98;
  fb->fillRoundRect(px - 16, py - 15, 32, 19, 8, H_PUMP3);
  fb->fillRoundRect(px - 13, py - 16, 26, 20, 8, H_PUMP2);
  fb->fillRoundRect(px - 7, py - 17, 14, 21, 6, H_PUMP);
  fb->fillRect(px - 1, py - 21, 3, 5, H_STEM);
  const uint8_t gl = bright ? H_GLOW : (((now / 90) % 5) ? H_GLOW2 : H_GLOW);
  fb->fillTriangle(px - 10, py - 6, px - 4, py - 6, px - 7, py - 11, gl);
  fb->fillTriangle(px + 4, py - 6, px + 10, py - 6, px + 7, py - 11, gl);
  fb->fillTriangle(px - 2, py - 3, px + 2, py - 3, px, py - 6, gl);
  fb->fillRect(px - 10, py - 1, 20, 3, gl);
  for (int k = -9; k < 10; k += 4) fb->fillRect(px + k, py - 2, 2, 1, gl);
  fb->fillRect(px - 7, py + 2, 2, 1, gl); fb->fillRect(px + 3, py + 2, 2, 1, gl);
  // kleiner Kuerbis
  fb->fillRoundRect(106, 92, 12, 9, 4, H_PUMP2); fb->fillRoundRect(109, 91, 6, 10, 3, H_PUMP);
  fb->drawPixel(112, 90, H_STEM); fb->drawPixel(112, 89, H_STEM);

  // ---- Nebel ----
  for (int i = 0; i < 40; i++) {
    const int x = (int)((i * 37 + now / 60) % 170) - 5, y = 102 + (i * 7) % 16;
    fb->drawFastHLine(x, y, 6 + (i % 4) * 2, H_FOG);
  }

  // ---- Spinne ----
  const int spy = 30 + (int)((sinf((float)(now % 9000) / 9000.0f * 6.2832f) + 1.0f) * 14.0f);
  fb->drawFastVLine(44, 0, spy, H_WEB);
  fb->fillCircle(44, spy + 2, 2, H_SPID);
  for (int k = -1; k <= 1; k += 2) {
    fb->drawLine(44, spy + 1, 44 + k * 4, spy - 1, H_SPID); fb->drawLine(44, spy + 2, 44 + k * 4, spy + 3, H_SPID);
    fb->drawLine(44, spy + 3, 44 + k * 4, spy + 6, H_SPID);
  }
  fb->drawPixel(43, spy + 2, H_GLOW2); fb->drawPixel(45, spy + 2, H_GLOW2);

  // ---- Fledermaeuse ----
  for (int i = 0; i < 4; i++) {
    Bat& b = bats[i];
    b.x += b.v * dt; b.ph += dt * 3.0f;
    if (b.x > FB_W + 8) { b.x = -8; b.y = 38.0f + (float)rn(44); b.v = 14.0f + rf() * 12.0f; }
    bat((int)b.x, (int)(b.y + sinf(b.ph) * b.amp), ((now / 110 + (uint32_t)i) & 1) != 0);
  }

  // ---- Gespenst ----
  const float bt = booT ? secs(now, booT) : 99.0f;
  if (bt < 2.2f) {                                               // "BUH!": heran sausen, gross werden
    const float a = fcl(bt / 0.6f, 0.0f, 1.0f);
    const int s = a < 0.5f ? 1 : 2;
    const int gx2 = (int)(150.0f - a * 90.0f), gy2 = (int)(64.0f + a * 6.0f);
    ghost(gx2 - (s == 2 ? 6 : 0), gy2, s, now);
    if (bt > 0.5f) {
      fb->fillRoundRect(gx2 + 24, gy2 - 2, 30, 13, 4, H_GHOST);
      fb->fillTriangle(gx2 + 24, gy2 + 6, gx2 + 28, gy2 + 6, gx2 + 19, gy2 + 10, H_GHOST);
      txt(gx2 + 28, gy2 + 1, "BUH!", H_EYE);
    }
  } else {
    const float gt = (float)(now % 12000) / 12000.0f * 6.2832f;
    ghost(74 + (int)(sinf(gt) * 14.0f), 66 + (int)(sinf(gt * 3.0f) * 3.0f), 1, now);
  }

  // ---- Uhrzeit ----
  drawTime(49, 4, 4, c, cellS);
  drawTime(49, 4, 4, c, cellD);
  if (c.showSec && c.timeOk) fb->drawFastHLine(49, 33, (c.ss + 1) * 108 / 60, H_DIM);

  // ---- Countdown ----
  char b[24];
  const int cx = 104;
  if (c.timeOk) {
    if (c.mon == 10 && c.mday == 31) {
      const uint8_t a = ((now / 500) & 1) ? H_ORG : H_GRN;
      txtS(cx - txtw("HAPPY", 2) / 2, 38, "HAPPY", a, H_SH, 2);
      txtS(cx - txtw("HALLOWEEN", 1) / 2, 55, "HALLOWEEN", H_TXT, H_SH);
    } else {
      const int d = daysUntil(c.year, c.mon, c.mday, 10, 31);
      snprintf(b, sizeof(b), "%d", d);
      const char* unit = d == 1 ? "TAG" : "TAGE";
      const int w = txtw(b, 2) + 5 + txtw(unit, 1), x0 = cx - w / 2;
      txtS(x0, 38, b, H_ORG, H_SH, 2);
      txtS(x0 + txtw(b, 2) + 5, 45, unit, H_TXT, H_SH);
      txtS(cx - txtw("BIS HALLOWEEN") / 2, 55, "BIS HALLOWEEN", H_DIM, H_SH);
    }
    snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
    txt(4, 110, b, H_DIM);
  }
  if (c.hasWx) tempTxt(100 - tempW(c.temp, 1), 110, c.temp, H_DIM);
}

static void cover(float p, bool, uint32_t now) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  const int top = FB_H - (int)(p * (FB_H + 16));
  for (int y = 0; y < FB_H; y++) {
    const int e = top + (int)(sinf((float)y * 0.3f + (float)now * 0.004f) * 4.0f);
    for (int x = 0; x < FB_W; x++) {
      const int edge = e + (int)(sinf((float)x * 0.15f + (float)now * 0.003f) * 5.0f);
      if (y >= edge) bf[y * FB_W + x] = (y - edge < 2) ? H_DIM : H_FOG;
    }
  }
}

}  // namespace f_hal
