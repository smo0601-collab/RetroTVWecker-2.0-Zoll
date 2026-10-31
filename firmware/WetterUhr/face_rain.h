#pragma once
/*
  face_rain.h - Zifferblatt "Code-Regen"
  Gruener Zeichenregen in drei Tiefenstufen (hinten dunkel und langsam, vorn hell und
  schnell), gelegentliche Blitzer einzelner Spalten. In der Mitte steht die Uhr in einem
  freigeraeumten Feld, darunter ein Terminal mit Datum, Temperatur und Wind (Schreibmaschinen-
  Effekt). Zur vollen Minute laeuft ein Datenimpuls ueber den Schirm, alle 20 Sekunden ein
  langsamer Abtastbalken, zur vollen Stunde verrutschen Bildzeilen (Glitch).
  Der Regen wird nie komplett neu aufgebaut, nur Zeichen und Spalten bewegen sich.
  Uebergang: Regenvorhang.
*/
#include "faces_core.h"

namespace f_rain {
using namespace fc;

#define R_COLS 26
#define R_ROWS 15
enum : uint8_t { R_HEAD = 19, R_EDGE, R_TXT, R_TXT2, R_CORE, R_GLOW, R_HALO, R_BAND };

struct Col { float head, spd; uint8_t len, depth; };
static Col col[R_COLS];
static const char GL[] = "0123456789ABCDEFGHJKLMNPRSTUVXYZ+-=<>/|:*#";
static uint32_t lastT, tEnter, flashUntil, pulseT, scanT, glitchUntil, nextFlash;
static uint8_t flashCol, lastMm, lastHh;

static void reset(Col& k, bool first) {
  k.depth = rn(3);
  static const float SP[3] = {5.0f, 9.0f, 15.0f};
  k.spd = SP[k.depth] * (0.75f + rf() * 0.6f);
  k.len = 8 + rn(10);
  k.head = first ? (float)rn(R_ROWS + 8) - 2.0f : -(float)rn(4);
}

static void enter(uint32_t now) {
  static const float F[6] = {1.0f, 0.78f, 0.56f, 0.40f, 0.28f, 0.18f};
  static const float D[3] = {0.40f, 0.70f, 1.0f};
  for (int d = 0; d < 3; d++)
    for (int t = 0; t < 6; t++) {
      const float f = D[d] * F[t];
      pal(1 + d * 6 + t, (uint8_t)(20 * f), (uint8_t)(255 * f), (uint8_t)(70 * f));
    }
  pal(R_HEAD, 200, 255, 215); pal(R_EDGE, 0, 160, 60); pal(R_TXT, 0, 235, 95); pal(R_TXT2, 0, 120, 46);
  pal(R_CORE, 170, 255, 195); pal(R_GLOW, 0, 150, 56); pal(R_HALO, 0, 56, 24); pal(R_BAND, 0, 80, 36);
  for (int i = 0; i < R_COLS; i++) reset(col[i], true);
  lastT = tEnter = now; flashUntil = pulseT = scanT = glitchUntil = 0; nextFlash = now + 1200;
  flashCol = 0; lastMm = lastHh = 255;
}

static void haloFn(int x, int y, int s, int, int) { fb->fillRect(x - 1, y - 1, s + 2, s + 2, R_HALO); }
static void glowFn(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, R_GLOW); }
static void coreFn(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s - 2, s - 2, R_CORE); }

// wie viele Zeichen noch geschrieben werden duerfen (Schreibmaschine)
static int typed(int x, int y, const char* s, uint8_t colr, int& budget) {
  const int n = (int)strlen(s);
  const int k = budget < n ? (budget < 0 ? 0 : budget) : n;
  char t[32];
  const int m = k < 31 ? k : 31;
  memcpy(t, s, m);
  t[m] = 0;
  txt(x, y, t, colr);
  budget -= n;
  return k;
}

static bool boosted(int py, uint32_t now) {
  if (pulseT) {
    const float y = secs(now, pulseT) / 0.7f * 140.0f - 10.0f;
    if (fabsf(py - y) < 9.0f) return true;
  }
  if (scanT) {
    const float y = secs(now, scanT) / 2.0f * 140.0f - 10.0f;
    if (fabsf(py - y) < 6.0f) return true;
  }
  return false;
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = now;

  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) pulseT = now ? now : 1;
    if (lastHh != 255 && c.hh != lastHh) glitchUntil = now + 500;
    lastMm = c.mm; lastHh = c.hh;
  }
  if (!scanT && (now / 1000) % 20 == 10 && (now % 1000) < 120) scanT = now ? now : 1;
  if (pulseT && secs(now, pulseT) > 0.8f) pulseT = 0;
  if (scanT && secs(now, scanT) > 2.1f) scanT = 0;
  if (now >= nextFlash) { flashCol = rn(R_COLS); flashUntil = now + 160; nextFlash = now + 900 + rn(1800); }

  fb->fillScreen(0);
  for (int ci = 0; ci < R_COLS; ci++) {
    Col& k = col[ci];
    k.head += k.spd * dt;
    if (k.head - k.len > R_ROWS) { reset(k, false); }
    const int cx = 2 + ci * 6;
    const int hr = (int)floorf(k.head);
    const bool fl = ci == flashCol && now < flashUntil;
    for (int j = 0; j < k.len; j++) {
      const int row = hr - j;
      if (row < 0 || row >= R_ROWS) continue;
      uint8_t idx;
      if (j == 0) idx = k.depth == 0 ? 1 : R_HEAD;
      else { int t = j * 6 / k.len; if (t > 5) t = 5; idx = (uint8_t)(1 + k.depth * 6 + t); }
      if (fl || boosted(row * 8 + 4, now)) idx = R_HEAD;
      uint32_t h = (uint32_t)(ci * 73856093u) ^ (uint32_t)(row * 19349663u);
      h ^= h >> 13; h *= 0x5bd1e995u; h ^= h >> 15;
      const uint32_t g = h + (((h & 3) == 0) ? (now >> 7) : 0);
      fb->drawChar(cx, row * 8, GL[g % (sizeof(GL) - 1)], idx, idx, 1);
    }
  }
  if (pulseT) {                                              // Datenimpuls: helle Linie
    const int y = (int)(secs(now, pulseT) / 0.7f * 140.0f - 10.0f);
    if (y >= 0 && y < FB_H) fb->drawFastHLine(0, y, FB_W, R_BAND);
  }

  // Uhrfeld
  fb->fillRect(18, 24, 124, 42, 0);
  fb->drawRect(18, 24, 124, 42, R_EDGE);
  fb->drawFastHLine(22, 24, 12, R_HEAD); fb->drawFastVLine(18, 28, 8, R_HEAD);
  fb->drawFastHLine(126, 65, 12, R_HEAD); fb->drawFastVLine(141, 55, 8, R_HEAD);
  drawTime(26, 31, 4, c, haloFn);
  drawTime(26, 31, 4, c, glowFn);
  drawTime(26, 31, 4, c, coreFn);

  // Terminal
  fb->fillRect(18, 70, 124, 36, 0);
  fb->drawRect(18, 70, 124, 36, R_EDGE);
  int budget = (int)((now - tEnter) / 32);
  char b[32];
  if (c.timeOk) { char d[16]; langDateY(d, sizeof(d), c.mday, c.mon, c.year); snprintf(b, sizeof(b), "> %s %s", WD_SHORT[c.wday % 7], d); }
  else          snprintf(b, sizeof(b), "> --");
  typed(23, 74, b, R_TXT, budget);
  if (c.hasWx) {
    snprintf(b, sizeof(b), "> TEMP ");
    const int k = typed(23, 84, b, R_TXT, budget);
    if (k >= 7) tempTxt(23 + 7 * 6, 84, c.temp, R_TXT);
    budget -= 6;
    snprintf(b, sizeof(b), "H%d%%", c.hum);
    typed(23 + 7 * 6 + tempW(c.temp, 1) + 6, 84, b, R_TXT2, budget);
    snprintf(b, sizeof(b), "> WIND %dKMH", c.wind);
  } else {
    typed(23, 84, "> TEMP --", R_TXT, budget);
    snprintf(b, sizeof(b), "> WIND --");
  }
  const int kk = typed(23, 94, b, R_TXT, budget);
  if (budget >= 0 && ((now / 500) & 1)) txt(23 + kk * 6, 94, "_", R_HEAD);
  if (c.showSec && c.timeOk) { snprintf(b, sizeof(b), "SEC %02u", c.ss); txtr(137, 94, b, R_TXT2); }

  // Glitch zur vollen Stunde: Bildzeilen seitlich verschieben
  if (now < glitchUntil) {
    uint8_t* bf = buf();
    for (int n = 0; n < 5; n++) {
      const int y0 = rn(FB_H - 3);
      const int s = rn(19) - 9;
      for (int y = y0; y < y0 + 3; y++) {
        uint8_t* r = bf + y * FB_W;
        if (s > 0)      { memmove(r + s, r, FB_W - s); memset(r, 0, s); }
        else if (s < 0) { memmove(r, r - s, FB_W + s); memset(r + FB_W + s, 0, -s); }
      }
    }
  }
}

static void cover(float p, bool in, uint32_t) {
  uint8_t* bf = buf();
  for (int ci = 0; ci < R_COLS; ci++) {
    const int off = (int)(((uint32_t)(ci * 2654435761u)) >> 28) * 3;        // 0..45
    const int edge = (int)(p * (FB_H + 48)) - off;
    const int x0 = 2 + ci * 6;
    const int w = ci == R_COLS - 1 ? FB_W - x0 : 6;
    const int xs = ci == 0 ? 0 : x0;
    const int ww = ci == 0 ? x0 + w : w;
    if (!in) {                                        // oben nach unten schwarz
      const int e = imin(edge, FB_H);
      for (int y = 0; y < e; y++) memset(bf + y * FB_W + xs, 0, ww);
      if (edge > 0 && edge < FB_H) fb->fillRect(xs, edge - 1, ww, 1, R_HEAD);
    } else {                                          // unten bleibt schwarz, oben geben Regen frei
      const int s = imax(FB_H - edge, 0);
      for (int y = s; y < FB_H; y++) memset(bf + y * FB_W + xs, 0, ww);
      if (s > 0 && s < FB_H) fb->fillRect(xs, s, ww, 1, R_HEAD);
    }
  }
}

}  // namespace f_rain
