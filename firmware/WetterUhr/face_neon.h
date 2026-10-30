#pragma once
/*
  face_neon.h - Zifferblatt "Neon-Raster"
  Schwarzes Leuchtraster, ueber das Lichtimpulse wandern und an Kreuzungen abbiegen.
  Die Uhrzeit besteht aus leuchtenden Punkten (Kern + Schein). Ein orangefarbenes
  Lichtmotorrad umrundet den Bildrand einmal pro Minute (Sekunden) und zieht eine Spur.
  Jede Minute laeuft ein Lichtring von der Mitte nach aussen, Scanlines wie ein Roehrenbild.
  Uebergang: senkrechter Abtast-Balken.
*/
#include "faces_core.h"

namespace f_neon {
using namespace fc;

enum : uint8_t { N_GRID = 1, N_GRID2, N_HEAD, N_TRAIL1, N_TRAIL2, N_CORE, N_GLOW, N_HALO, N_ORG, N_ORG2, N_TXT, N_TXT2 };

struct Pulse { float pos, v; int8_t dir; bool horiz; uint8_t line; };
static Pulse pu[6];
static uint32_t lastT, ringT, ssT;
static uint8_t lastMm, lastSs;

static void respawn(Pulse& p) {
  p.horiz = rn(2);
  p.dir = rn(2) ? 1 : -1;
  p.line = p.horiz ? rn(8) : rn(10);
  p.v = 50.0f + rn(70);
  p.pos = p.dir > 0 ? -1.0f : (p.horiz ? 160.0f : 120.0f);
}

static void enter(uint32_t now) {
  pal(N_GRID, 0, 44, 62); pal(N_GRID2, 0, 84, 112); pal(N_HEAD, 190, 255, 255);
  pal(N_TRAIL1, 0, 200, 240); pal(N_TRAIL2, 0, 110, 150);
  pal(N_CORE, 210, 255, 255); pal(N_GLOW, 0, 160, 220); pal(N_HALO, 0, 58, 88);
  pal(N_ORG, 255, 150, 0); pal(N_ORG2, 120, 52, 0); pal(N_TXT, 0, 210, 240); pal(N_TXT2, 0, 110, 140);
  for (int i = 0; i < 6; i++) { respawn(pu[i]); pu[i].pos = (float)rn(120); }
  lastT = now; ringT = 0; ssT = now; lastMm = lastSs = 255;
}

static void haloFn(int x, int y, int s, int, int) { fb->fillRect(x - 1, y - 1, s + 2, s + 2, N_HALO); }
static void glowFn(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, N_GLOW); }
static void coreFn(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s - 2, s - 2, N_CORE); }

static void rim(float u, int& x, int& y) {          // Umfang des Rahmens (3,3)-(156,116)
  const float W = 153.0f, H = 113.0f, P = 2 * (W + H);
  float s = fmodf(u, 1.0f);
  if (s < 0) s += 1.0f;
  s *= P;
  if (s < W)           { x = 3 + (int)s;               y = 3; }
  else if (s < W + H)  { x = 156;                      y = 3 + (int)(s - W); }
  else if (s < 2*W+H)  { x = 156 - (int)(s - W - H);   y = 116; }
  else                 { x = 3;                        y = 116 - (int)(s - 2*W - H); }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = now;
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) ringT = now ? now : 1;
    lastMm = c.mm;
    if (c.ss != lastSs) { lastSs = c.ss; ssT = now; }
  }

  fb->fillScreen(0);
  for (int x = 0; x < FB_W; x += 16) fb->drawFastVLine(x, 0, FB_H, (x & 63) ? N_GRID : N_GRID2);
  fb->drawFastVLine(FB_W - 1, 0, FB_H, N_GRID2);
  for (int y = 0; y < FB_H; y += 15) fb->drawFastHLine(0, y, FB_W, (y % 60) ? N_GRID : N_GRID2);
  fb->drawFastHLine(0, FB_H - 1, FB_W, N_GRID2);

  // Lichtimpulse
  for (int i = 0; i < 6; i++) {
    Pulse& p = pu[i];
    const float old = p.pos;
    p.pos += p.dir * p.v * dt;
    const float cell = p.horiz ? 16.0f : 15.0f;
    const int a = (int)floorf(old / cell), b = (int)floorf(p.pos / cell);
    if (a != b && p.pos > 0 && rn(100) < 28) {                   // an der Kreuzung abbiegen
      const int cross = p.dir > 0 ? b : a;
      const int oldLine = p.line;
      const bool wasH = p.horiz;
      p.horiz = !wasH;
      p.line = (uint8_t)(wasH ? icl(cross, 0, 9) : icl(cross, 0, 7));
      p.pos = (float)(wasH ? oldLine * 15 : oldLine * 16);
      p.dir = rn(2) ? 1 : -1;
    }
    if (p.pos < -16.0f || p.pos > (p.horiz ? 176.0f : 136.0f)) { respawn(p); continue; }
    for (int k = 0; k < 14; k++) {
      const int q = (int)(p.pos - p.dir * k);
      const uint8_t col = k == 0 ? N_HEAD : (k < 5 ? N_TRAIL1 : N_TRAIL2);
      if (p.horiz) { const int y = p.line * 15; if (q >= 0 && q < FB_W) { fb->drawPixel(q, y, col); if (k == 0) fb->drawPixel(q, y + 1, col); } }
      else         { const int x = p.line * 16; if (q >= 0 && q < FB_H) { fb->drawPixel(x, q, col); if (k == 0) fb->drawPixel(x + 1, q, col); } }
    }
  }

  // Rahmen und Lichtmotorrad (Sekunden)
  float u;
  if (c.showSec && c.timeOk) {
    float fr = (float)(uint32_t)(now - ssT) * 0.001f;
    if (fr > 1.0f) fr = 1.0f;
    u = ((float)c.ss + fr) / 60.0f;
  } else u = (float)(now % 60000u) / 60000.0f;
  fb->drawRect(3, 3, 154, 114, N_GRID);
  for (int k = 30; k >= 1; k--) {
    int x, y;
    rim(u - k / 532.0f, x, y);
    fb->drawPixel(x, y, k < 5 ? N_ORG : N_ORG2);
  }
  {
    int x, y;
    rim(u, x, y);
    fb->fillRect(x - 1, y - 1, 3, 3, N_ORG);
    fb->drawPixel(x, y, N_HEAD);
  }

  // Zeit aus Leuchtpunkten
  const bool flick = ((now / 1000) % 11 == 0) && (((now / 55) & 3) == 0);
  drawTime(26, 46, 4, c, haloFn);
  drawTime(26, 46, 4, c, glowFn);
  if (!flick) drawTime(26, 46, 4, c, coreFn);

  // Eckanzeigen
  char b[24];
  const int e = 9;
  fb->drawFastHLine(1, 1, 7, N_TXT); fb->drawFastVLine(1, 1, 7, N_TXT);
  fb->drawFastHLine(FB_W - 8, 1, 7, N_TXT); fb->drawFastVLine(FB_W - 2, 1, 7, N_TXT);
  fb->drawFastHLine(1, FB_H - 2, 7, N_TXT); fb->drawFastVLine(1, FB_H - 8, 7, N_TXT);
  fb->drawFastHLine(FB_W - 8, FB_H - 2, 7, N_TXT); fb->drawFastVLine(FB_W - 2, FB_H - 8, 7, N_TXT);
  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(e + 1, 7, b, N_TXT);
  if (c.hasWx) {
    tempTxt(FB_W - e - 1 - tempW(c.temp, 1), 7, c.temp, N_TXT);
    snprintf(b, sizeof(b), "W %dKMH", c.wind);
    txt(e + 1, FB_H - 14, b, N_TXT2);
    snprintf(b, sizeof(b), "F %d%%", c.hum);
    txtr(FB_W - e - 4, FB_H - 14, b, N_TXT2);
  } else {
    txtr(FB_W - e - 1, 7, "--", N_TXT);
  }

  // Lichtring zur vollen Minute
  if (ringT) {
    const float r = (float)(uint32_t)(now - ringT) * 0.001f / 0.8f * 90.0f;
    if (r > 95.0f) ringT = 0;
    else {
      const int hw = (int)(r * 1.333f), hh = (int)r;
      fb->drawRect(80 - hw, 60 - hh, 2 * hw, 2 * hh, N_HEAD);
      if (r > 4) fb->drawRect(80 - hw + 3, 60 - hh + 3, 2 * hw - 6, 2 * hh - 6, N_TRAIL1);
      if (r > 8) fb->drawRect(80 - hw + 6, 60 - hh + 6, 2 * hw - 12, 2 * hh - 12, N_TRAIL2);
    }
  }
}

static void cover(float p, bool in, uint32_t) {
  const int x = (int)(p * (FB_W + 4));
  uint8_t* bf = buf();
  if (!in) {                                         // links wird schwarz, heller Balken laeuft nach rechts
    const int w = imin(x, FB_W);
    for (int y = 0; y < FB_H; y++) memset(bf + y * FB_W, 0, w);
    if (x < FB_W) fb->fillRect(x, 0, 2, FB_H, N_HEAD);
    if (x - 1 >= 0 && x - 1 < FB_W) fb->fillRect(imax(0, x - 3), 0, 2, FB_H, N_TRAIL1);
  } else {                                           // rechts schwarz, Balken gibt von links frei
    const int e = FB_W - x;
    for (int y = 0; y < FB_H; y++) memset(bf + y * FB_W + imax(e, 0), 0, FB_W - imax(e, 0));
    if (e >= 0 && e < FB_W) fb->fillRect(e, 0, 2, FB_H, N_HEAD);
  }
}

}  // namespace f_neon
