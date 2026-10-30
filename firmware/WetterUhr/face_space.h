#pragma once
/*
  face_space.h - Zifferblatt "Asteroiden"
  Vektorgrafik-Look: Sternenschichten mit Parallaxe, trudelnde Felsbrocken, die an den
  Raendern abprallen, und ein Schiff, das sich auf den naechsten Brocken ausrichtet und
  lenkende Geschosse abfeuert. Treffer zerbrechen Brocken in Splitter. Die Uhrzeit
  ist in Liniensegmenten gezeichnet. Zur vollen Stunde verliert das Schiff ein Leben.
  Uebergang: Roehren-Zusammenbruch (Bild schrumpft zu einer Linie).
*/
#include "faces_core.h"

namespace f_space {
using namespace fc;

enum : uint8_t { S_STAR1 = 1, S_STAR2, S_STAR3, S_LINE, S_ROCK, S_SHIP, S_FIRE, S_FRAG1, S_FRAG2, S_TXT, S_DIM };

struct Rock { float x, y, vx, vy, r, a, w; uint8_t v[9]; bool on; };
struct Bul  { float x, y, vx, vy, life; int8_t tgt; bool on; };
struct Frag { float x, y, vx, vy, life, a; };

static Rock rock[8];
static Bul  bul[3];
static Frag frag[24];
static float sx = 80, sa = 0;
static uint32_t lastT, nextFire, nextSpawn, hideUntil, scoreV;
static uint8_t lives, lastMm, lastHh;
static float starX[36];
static uint8_t starY[36];
static const uint8_t SEG[11] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x40};

static void spawnRock(float x, float y, float r) {
  for (int i = 0; i < 8; i++) {
    if (rock[i].on) continue;
    Rock& k = rock[i];
    k.on = true; k.x = x; k.y = y; k.r = r;
    const float a = rf() * 6.2832f, v = 8.0f + rf() * 16.0f;
    k.vx = cosf(a) * v; k.vy = sinf(a) * v;
    k.a = rf() * 6.2832f; k.w = (rf() - 0.5f) * 1.6f;
    for (int j = 0; j < 9; j++) k.v[j] = 72 + rn(48);
    return;
  }
}

static void spawnFrag(float x, float y, int n, float spd) {
  for (int i = 0; i < n; i++)
    for (int k = 0; k < 24; k++)
      if (frag[k].life <= 0) {
        const float a = rf() * 6.2832f, v = spd * (0.4f + rf());
        frag[k] = {x, y, cosf(a) * v, sinf(a) * v, 0.6f + rf() * 0.7f, rf() * 6.2832f};
        break;
      }
}

static void enter(uint32_t now) {
  pal(S_STAR1, 60, 64, 90); pal(S_STAR2, 120, 128, 170); pal(S_STAR3, 210, 220, 255);
  pal(S_LINE, 230, 238, 255); pal(S_ROCK, 170, 176, 196); pal(S_SHIP, 255, 255, 255);
  pal(S_FIRE, 255, 170, 60); pal(S_FRAG1, 255, 255, 255); pal(S_FRAG2, 110, 116, 140);
  pal(S_TXT, 190, 200, 235); pal(S_DIM, 100, 108, 140);
  for (int i = 0; i < 8; i++) rock[i].on = false;
  for (int i = 0; i < 3; i++) bul[i].on = false;
  for (int i = 0; i < 24; i++) frag[i].life = 0;
  for (int i = 0; i < 36; i++) { starX[i] = (float)rn(FB_W); starY[i] = rn(FB_H); }
  spawnRock(20, 30, 12); spawnRock(135, 85, 11); spawnRock(120, 18, 8); spawnRock(30, 95, 9);
  lastT = now; nextFire = now + 1500; nextSpawn = 0; hideUntil = 0;
  lives = 3; lastMm = lastHh = 255; scoreV = 1000 + rn(8000); sa = 0; sx = 80;
}

static void seg7(int x, int y, int w, int h, int d) {
  const uint8_t m = SEG[d > 10 ? 10 : d];
  const int h2 = h / 2;
  if (m & 1)  { fb->drawFastHLine(x + 1, y, w - 1, S_LINE);      fb->drawFastHLine(x + 1, y + 1, w - 1, S_LINE); }
  if (m & 2)  { fb->drawFastVLine(x + w, y + 1, h2 - 1, S_LINE); fb->drawFastVLine(x + w - 1, y + 1, h2 - 1, S_LINE); }
  if (m & 4)  { fb->drawFastVLine(x + w, y + h2 + 1, h2 - 1, S_LINE); fb->drawFastVLine(x + w - 1, y + h2 + 1, h2 - 1, S_LINE); }
  if (m & 8)  { fb->drawFastHLine(x + 1, y + h, w - 1, S_LINE);  fb->drawFastHLine(x + 1, y + h - 1, w - 1, S_LINE); }
  if (m & 16) { fb->drawFastVLine(x, y + h2 + 1, h2 - 1, S_LINE); fb->drawFastVLine(x + 1, y + h2 + 1, h2 - 1, S_LINE); }
  if (m & 32) { fb->drawFastVLine(x, y + 1, h2 - 1, S_LINE);     fb->drawFastVLine(x + 1, y + 1, h2 - 1, S_LINE); }
  if (m & 64) { fb->drawFastHLine(x + 1, y + h2, w - 1, S_LINE); fb->drawFastHLine(x + 1, y + h2 - 1, w - 1, S_LINE); }
}

static void shipShape(float x, float y, float a, uint8_t col) {
  const float nx = sinf(a), ny = -cosf(a);
  const float lx = sinf(a + 2.5f), ly = -cosf(a + 2.5f), rx = sinf(a - 2.5f), ry = -cosf(a - 2.5f);
  fb->drawLine((int)(x + nx * 7), (int)(y + ny * 7), (int)(x + lx * 6), (int)(y + ly * 6), col);
  fb->drawLine((int)(x + nx * 7), (int)(y + ny * 7), (int)(x + rx * 6), (int)(y + ry * 6), col);
  fb->drawLine((int)(x + lx * 4), (int)(y + ly * 4), (int)(x + rx * 4), (int)(y + ry * 4), col);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = now;
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm && now >= hideUntil) nextFire = now;
    if (lastHh != 255 && c.hh != lastHh) {
      spawnFrag(sx, 100, 16, 50);
      hideUntil = now + 1500; lives = lives > 1 ? lives - 1 : 3;
    }
    lastMm = c.mm; lastHh = c.hh;
  }

  fb->fillScreen(0);
  // Sterne (Parallaxe)
  for (int i = 0; i < 36; i++) {
    const int layer = i % 3;
    starX[i] -= (4.0f + layer * 6.0f) * dt;
    if (starX[i] < 0) { starX[i] += FB_W; starY[i] = rn(FB_H); }
    fb->drawPixel((int)starX[i], starY[i], S_STAR1 + layer);
  }

  // Brocken
  int active = 0;
  for (int i = 0; i < 8; i++) {
    Rock& k = rock[i];
    if (!k.on) continue;
    active++;
    k.x += k.vx * dt; k.y += k.vy * dt; k.a += k.w * dt;
    if (k.x < k.r && k.vx < 0) k.vx = -k.vx;
    if (k.x > FB_W - k.r && k.vx > 0) k.vx = -k.vx;
    if (k.y < k.r + 8 && k.vy < 0) k.vy = -k.vy;
    if (k.y > FB_H - k.r - 8 && k.vy > 0) k.vy = -k.vy;
    int px = 0, py = 0;
    for (int j = 0; j <= 9; j++) {
      const int jj = j % 9;
      const float an = k.a + jj * 0.6981f;
      const float rr = k.r * k.v[jj] * 0.01f;
      const int qx = (int)(k.x + cosf(an) * rr), qy = (int)(k.y + sinf(an) * rr);
      if (j) fb->drawLine(px, py, qx, qy, S_ROCK);
      px = qx; py = qy;
    }
  }
  if (active < 3 && now >= nextSpawn) {
    nextSpawn = now + 1800;
    const int side = rn(4);
    spawnRock(side == 0 ? 14.0f : (side == 1 ? 146.0f : (float)(20 + rn(120))), side == 2 ? 24.0f : (side == 3 ? 90.0f : (float)(24 + rn(60))), 12);
  }

  // Schiff
  const float shipY = 100.0f;
  const bool alive = now >= hideUntil;
  sx = 80.0f + 36.0f * sinf(now * 0.00035f);
  if (alive) {
    int ti = -1; float bd = 1e9f;
    for (int i = 0; i < 8; i++) if (rock[i].on) {
      const float dx = rock[i].x - sx, dy = rock[i].y - shipY, d = dx * dx + dy * dy;
      if (d < bd) { bd = d; ti = i; }
    }
    if (ti >= 0) {
      const float want = atan2f(rock[ti].x - sx, -(rock[ti].y - shipY));
      float diff = want - sa;
      while (diff > 3.1416f) diff -= 6.2832f;
      while (diff < -3.1416f) diff += 6.2832f;
      const float step = 2.2f * dt;
      sa += fabsf(diff) < step ? diff : (diff > 0 ? step : -step);
      if (now >= nextFire && fabsf(diff) < 0.2f) {
        for (int k = 0; k < 3; k++)
          if (!bul[k].on) {
            bul[k] = {sx + sinf(sa) * 8, shipY - cosf(sa) * 8, sinf(sa) * 125.0f, -cosf(sa) * 125.0f, 2.5f, (int8_t)ti, true};
            break;
          }
        nextFire = now + 1700 + rn(1200);
      }
    }
    shipShape(sx, shipY, sa, S_SHIP);
    if (((now / 90) & 1) && fabsf(sinf(now * 0.00035f)) > 0.1f) {          // Schubflamme
      const float bx = sx - sinf(sa) * 6, by = shipY + cosf(sa) * 6;
      fb->drawLine((int)bx, (int)by, (int)(bx - sinf(sa) * 3), (int)(by + cosf(sa) * 3), S_FIRE);
    }
  }

  // Geschosse
  for (int i = 0; i < 3; i++) {
    Bul& b = bul[i];
    if (!b.on) continue;
    b.life -= dt;
    if (b.tgt >= 0 && rock[b.tgt].on) {                       // lenken
      const float want = atan2f(rock[b.tgt].y - b.y, rock[b.tgt].x - b.x);
      float cur = atan2f(b.vy, b.vx), diff = want - cur;
      while (diff > 3.1416f) diff -= 6.2832f;
      while (diff < -3.1416f) diff += 6.2832f;
      const float st = 2.6f * dt;
      cur += fabsf(diff) < st ? diff : (diff > 0 ? st : -st);
      b.vx = cosf(cur) * 125.0f; b.vy = sinf(cur) * 125.0f;
    }
    b.x += b.vx * dt; b.y += b.vy * dt;
    if (b.life <= 0 || b.x < 0 || b.x >= FB_W || b.y < 0 || b.y >= FB_H) { b.on = false; continue; }
    fb->fillRect((int)b.x, (int)b.y, 2, 2, S_SHIP);
    for (int j = 0; j < 8; j++) {
      Rock& k = rock[j];
      if (!k.on) continue;
      const float dx = k.x - b.x, dy = k.y - b.y;
      if (dx * dx + dy * dy < (k.r + 2) * (k.r + 2)) {
        k.on = false; b.on = false;
        scoreV += k.r > 10 ? 20 : (k.r > 6 ? 50 : 100);
        spawnFrag(k.x, k.y, 8, 40);
        if (k.r >= 9) { spawnRock(k.x, k.y, k.r * 0.62f); spawnRock(k.x, k.y, k.r * 0.62f); }
        break;
      }
    }
  }

  // Splitter
  for (int i = 0; i < 24; i++) {
    Frag& f = frag[i];
    if (f.life <= 0) continue;
    f.life -= dt; f.x += f.vx * dt; f.y += f.vy * dt; f.a += dt * 4.0f;
    if (f.x < 0 || f.x >= FB_W || f.y < 0 || f.y >= FB_H) { f.life = 0; continue; }
    fb->drawLine((int)f.x, (int)f.y, (int)(f.x + cosf(f.a) * 3), (int)(f.y + sinf(f.a) * 3), f.life > 0.35f ? S_FRAG1 : S_FRAG2);
  }

  // Uhrzeit aus Linien: 4 Ziffern + Doppelpunkt
  const int d0 = c.timeOk ? c.hh / 10 : 10, d1 = c.timeOk ? c.hh % 10 : 10;
  const int d2 = c.timeOk ? c.mm / 10 : 10, d3 = c.timeOk ? c.mm % 10 : 10;
  const int x0 = 25, y0 = 44, w = 18, h = 30;
  seg7(x0, y0, w, h, d0);
  seg7(x0 + 26, y0, w, h, d1);
  seg7(x0 + 66, y0, w, h, d2);
  seg7(x0 + 92, y0, w, h, d3);
  if (!c.timeOk || (c.ss & 1) == 0) {
    fb->fillRect(x0 + 55, y0 + 9, 2, 2, S_LINE);
    fb->fillRect(x0 + 55, y0 + 21, 2, 2, S_LINE);
  }
  if (c.showSec && c.timeOk) {
    char b[4];
    snprintf(b, sizeof(b), "%02u", c.ss);
    txt(142, y0 + h - 7, b, S_DIM);
  }

  // Anzeigen
  for (int i = 0; i < lives; i++) shipShape(8.0f + i * 10, 8, 0, S_SHIP);
  char b[24];
  snprintf(b, sizeof(b), "%05lu", (unsigned long)(scoreV % 100000));
  txtr(FB_W - 4, 4, b, S_TXT);
  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 111, b, S_DIM);
  if (c.hasWx) tempTxt(FB_W - 10 - tempW(c.temp, 1), 111, c.temp, S_DIM);
  else         txtr(FB_W - 10, 111, "--", S_DIM);
}

static void cover(float p, bool, uint32_t) {
  uint8_t* bf = buf();
  const float q = fcl(p / 0.8f, 0.0f, 1.0f);                 // 0..0.8: vertikal zusammenfallen
  const int half = (int)((1.0f - q) * 60.0f);
  const int top = 60 - half, bot = 60 + half;
  if (top > 0) memset(bf, 0, top * FB_W);
  if (bot < FB_H) memset(bf + bot * FB_W, 0, (FB_H - bot) * FB_W);
  if (q >= 1.0f) {                                           // 0.8..1: Linie zieht sich zum Punkt zusammen
    memset(bf, 0, FB_W * FB_H);
    const float s = fcl((p - 0.8f) / 0.2f, 0.0f, 1.0f);
    const int len = (int)((1.0f - s) * 80.0f);
    if (len > 0) fb->fillRect(80 - len, 59, 2 * len, 2, S_SHIP);
    else if (s < 1.0f) fb->fillRect(79, 59, 2, 2, S_SHIP);
  } else if (half < 60) {
    fb->drawFastHLine(0, top, FB_W, S_SHIP);
    fb->drawFastHLine(0, bot - 1, FB_W, S_SHIP);
  }
}

}  // namespace f_space
